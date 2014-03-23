#lang racket


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define args (current-command-line-arguments))

(define program (open-input-file (vector-ref args 0)))
(define store-matrix (open-input-file (vector-ref args 1)))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;; Set the degree of context-sensitivity used
(define k 1)


;; Read the desugared source tree and flatten it, assigning a number to each language form

(define label 0)
(define saved (make-hash))

(define (save! e)
        (define l label)
        (set! label (+ label 1))
        (hash-set! saved l e)
        l)

(define (process-input e)
        (match e
               [`(prim ,op ,ae* ...)
                (save! `(prim ,op . ,(map process-input ae*)))]
               [`(halt)
                (save! `(halt))]
               [`(set!/k ,x ,aev ,aek)
                (save! `(set!/k ,(process-input x) ,(process-input aev) ,(process-input aek)))]
               [`(if ,ae ,et ,ef)
                (save! `(if ,(process-input ae) ,(process-input et) ,(process-input ef)))]
               [`(lambda ,args ,eb)
                (save! `(lambda ,(map process-input args) ,(process-input eb)))]
               [`(,ae* ...)
                (save! (map process-input ae*))]
               [else (save! e)]))

(define (pretty-program [l root])
        (define e (hash-ref saved l))
        (match e
               [`(halt)
                `(label ,l `(halt))]
               [`(prim ,op ,ae* ...)
                `(label ,l (prim ,op . ,(map pretty-program ae*)))]
               [`(set!/k ,x ,aev ,aek)
                `(label ,l (set!/k ,(pretty-program x) ,(pretty-program aev) ,(pretty-program aek)))]
               [`(if ,ae ,et ,ef)
                `(label ,l (if ,(pretty-program ae) ,(pretty-program et) ,(pretty-program ef)))]
               [`(lambda ,args ,eb)
                `(label ,l (lambda ,(map pretty-program args) ,(pretty-program eb)))]
               [(? list?)
                `(label ,l ,(map pretty-program e))]
               [else `(label ,l ,e)]))


(define root (process-input (read program)))



;; Utility for iterating over the program
(define (iter callback! [l root])
        (define e (hash-ref saved l))
        (callback! l e)
        (match e
               [`(halt)
                 (void)]
               [`(prim ,op ,ae* ...)
                 (map (lambda (l) (iter callback! l)) ae*)]
               [`(set!/k ,x ,aev ,aek)
                 (iter callback! x)
                 (iter callback! aev)
                 (iter callback! aek)]
               [`(if ,ae ,et ,ef)
                 (iter callback! ae)
                 (iter callback! et)
                 (iter callback! ef)]
               [`(lambda ,args ,eb)
                 (iter callback! eb)
                 (map (lambda (l) (iter callback! l)) args)]
               [(? list?)
                (map (lambda (l) (iter callback! l)) e)]
               [else (void)])
        (void))



; ARGSN
(define ARGSN 0)
(define (build-ARGSN! l e)
        (when (not (or (not (list? e))
                       (member (first e) '(prim set!/k if lambda halt))))
              ; e is a callsite
              (when (> (- (length e) 1) 0)
                    (set! ARGSN (max ARGSN (- (length e) 1))))))
(iter build-ARGSN!)


;; Build X

(define X '())
(define (build-X! l e)
        (when (and (symbol? e) (not (equal? e 'halt)))
              (when (not (member e X))
                    (set! X (cons e X)))))
(iter build-X!)



;; Build L and T

(define L (set))
(define (build-L! l e)
        (when (not (or (not (list? e))
                       (member (first e) '(prim set!/k if lambda halt))))
              ; e is a callsite
              (set! L (set-add L l))))
(iter build-L!)

(define (gen-T t ck)
        (if (= ck k)
            t
            (gen-T (append* (map (lambda (ts) (foldl (lambda (l ts*)
                                                             (cons (cons l ts) ts*)) 
                                                     '() 
                                                     (set->list L))) 
                                 t)) 
                   (+ ck 1))))

(define T (gen-T '(()) 0))
(define (build-offs off rem [h (hash)])
        (if (null? rem)
            h
            (build-offs (+ 1 off) (cdr rem) (hash-set h (car rem) off))))
(define T-offs (build-offs 0 T))


;; Build LAM and FREE and V

(define LAM (set))
(define (build-LAM! l e)
        (when (and (list? e) (equal? (first e) 'lambda))
              ; e is a lambda-abstraction
              (set! LAM (set-add LAM l))))
(iter build-LAM!)

(define FREE (make-hash))
(define (free l [def (set)])
        (define e (hash-ref saved l))
        (match e
               [`(prim ,op ,ae* ...)
                 (apply set-union (map (lambda (ae) (free ae def)) ae*))]
               [`(set!/k ,x ,aev ,aek)
                 (set-union (free x def) (free aev def) (free aek def))]
               [`(if ,ae ,et ,ef)
                 (set-union (free ae def) (free et def) (free ef def))]
               [`(lambda ,args ,eb)
                 (free eb (set-union def (apply set (map (lambda (arg) (hash-ref saved arg)) args))))]
               ['(halt) (set)]
               [(? list?)
                (apply set-union (map (lambda (ae) (free ae def)) e))] 
               ['halt (set)]
               [(? symbol?)
                (set-subtract (set e) def)] 
               [else (set)]))
(define (build-FREE! l e)
        (when (and (list? e) (equal? (first e) 'lambda))
              ; e is a lambda-abstraction
              (hash-set! FREE l (set->list (free l)))))
(iter build-FREE!)

(define B '(VOID TRUE FALSE INT))
(define (gen-Envs envs frees)
        (if (null? frees)
            envs
            (gen-Envs (append* (map (lambda (env) (foldl (lambda (ts env*)
                                                                 (cons (hash-set env (car frees) ts) env*)) 
                                                         '() 
                                                         T))
                                    envs))
                      (cdr frees))))
(define CLO (foldl (lambda (lam clos) 
                           (append clos
                                   (map (lambda (env) `(,lam ,env))
                                        (gen-Envs `(,(hash)) (hash-ref FREE lam)))))
                   '()
                   (set->list LAM)))
(define CLO-offs (build-offs 0 CLO))

(define V (append CLO B))


;; Build A from X*T + V
(define X*T (append* (map (lambda (x)
                                  (foldl (lambda (t x*)
                                                 (cons `(,x ,t) x*))
                                         '()
                                         (reverse T)))
                          X)))



(define A (append X*T V))


;; Build C and S

(define C '())

(define (addC! l env)
        (define envs (gen-Envs `(,(hash)) (set->list (set-intersect env (free l)))))
        (set! C (append C (map (lambda (env)
                                                `(,l ,env))
                                        envs))))

(define (iterC l env)
        (define e (hash-ref saved l))
        (match e
               [`(halt)
                 (addC! l env)]
               [`(prim ,op ,ae* ...)
                 (addC! l env)
                 (map (lambda (l) (iterC l env)) ae*)]
               [`(set!/k ,x ,aev ,aek)
                 (addC! l env)
                 (iterC x env)
                 (iterC aev env)
                 (iterC aek env)]
               [`(if ,ae ,et ,ef)
                 (addC! l env)
                 (iterC ae env)
                 (iterC et env)
                 (iterC ef env)]
               [`(lambda ,args ,eb)
                 (iterC eb (foldl (lambda (arg env+) (set-add env+ (hash-ref saved arg))) env args))]
               [(? list?)
                 (addC! l env)
                 (map (lambda (l) (iterC l env)) e)]
               [else (void)])
        (void))
(iterC root (set))

(define S (append* (map (lambda (c)
                                (foldl (lambda (t s*)
                                               (cons (append c `(,t)) s*))
                                       '()
                                       (reverse T)))
                        C)))

(define C-offs (build-offs 0 C))
(define S-offs (build-offs 0 S))

; lengths
(define lenV (length V))
(define lenX*T (length X*T))
(define lenA (length A))
(define lenC (length C))
(define lenS (length S))
(define lenT (length T))
(define lenCLO (length CLO))
(define lenX (length X))



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(display "Labels: ")
(pretty-print saved)
(display "States: ")
(pretty-print S)
(display "Vars: ")
(pretty-print X)
(display "Cols/Values: ")
(pretty-print V)
(display "Rows/Addrs: ")
(pretty-print A)
(newline)
(newline)

(define (print-store in)
  (define col (read in))
  (when (not (eof-object? col))
    (define row (read in))
    
    ; should only print if row < (length X*T)
    (when (< row lenX*T)
      (define addr (list-ref X*T row))
      (define value (list-ref V col))
      
      (define var (car addr))
      (define time (cadr addr))
      
      (printf "~a,~a ==> ~a~n" var time value))
    
    (print-store in)))

; read the matrix name and number of rows and columns
(define _ (read store-matrix))
(define store-num-rows (read store-matrix))
(define store-num-cols (read store-matrix))
(print-store store-matrix)
