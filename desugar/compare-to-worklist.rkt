#lang racket


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define args (current-command-line-arguments))

(define program (open-input-file (vector-ref args 0)))
(define store-matrix (open-input-file (vector-ref args 1)))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



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
(define ARGSN 1)
(define (build-ARGSN! l e)
        (when (not (or (not (list? e))
                       (member (first e) '(prim set!/k if lambda halt))))
              ; e is a callsite
              (when (> (- (length e) 1) 0)
                    (set! ARGSN (max ARGSN (- (length e) 1))))))
(iter build-ARGSN!)


;; Build S

(define S '())
(define (build-S! l e)
        (when (and (list? e) (not (equal? (first e) 'lambda)))
              (set! S (cons l S))))
(iter build-S!)


;; Build X

(define X '())
(define (build-X! l e)
        (when (and (symbol? e) (not (equal? e 'halt)))
              (when (not (member e X))
                    (set! X (cons e X)))))
(iter build-X!)


;; Build LAM and FREE and V

(define LAM (set))
(define (build-LAM! l e)
        (when (and (list? e) (equal? (first e) 'lambda))
              ; e is a lambda-abstraction
              (set! LAM (set-add LAM l))))
(iter build-LAM!)
(define L (set->list LAM))


(define INTLOCS (set))
(define (build-INTLOCS! l e)
        (when (number? e)
              ; e is a literal
              (set! INTLOCS (set-add INTLOCS l))))
(iter build-INTLOCS!)
(define I (set->list INTLOCS))


(define B '(LIST VOID TRUE FALSE INT))
(define V (append L I B))
(define A (append X V))


(define allprims (set))
(define (build-allprims! l e)
        (when (and (list? e) (equal? (first e) 'prim))
              (set! allprims (set-add allprims (second e)))))
(iter build-allprims!)


; lengths
(define lenV (length V)) 
(define lenA (length A)) 
(define lenS (length S)) 
(define lenX (length X))





;;;;;;;;;;;;;;;;;;;;;;; WORKLIST ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(define (atomic ael store)
        (define ae (hash-ref saved ael))
        (match ae
               [#t (set 'TRUE)]
               [#f (set 'FALSE)]
               [(? number?) (set ael)]
               [(? symbol?) (hash-ref store ae)]
               [`(lambda ,args ,eb) (set ae)]))


(define (delta op aevs)
        (match (case op ((+ - * /) 'NUM) ((void print) 'VOID) ((< > <= >= = null? equal? not) 'BOOL) ((list append cons cdr car) 'LIST))
               ['NUM (set 'INT)]
               ['VOID (set 'VOID)]
               ['BOOL (set 'TRUE 'FALSE)]
               ['LIST (set-union (foldl set-union (set) aevs) (set 'LIST))]))


(define (store-join st1 st2)
        (foldl (lambda (k sigma)
                       (hash-set sigma k (set-union (hash-ref st1 k (lambda () (set))) 
                                                    (hash-ref st2 k (lambda () (set))))))
               (hash)
               (set->list (set-union (list->set (hash-keys st1)) (list->set (hash-keys st2))))))


(define (successors state)
        (match state [`(,l ,sigma)
                      (define e (hash-ref saved l))
                      (match e
                             [`(halt) '()]
                             [`(prim ,op ,aes ... ,aek)
                              (define fv (atomic aek sigma)) 
                              (define aevs (map (lambda (ae) (atomic ae sigma)) aes)) 
                              (define clos (filter (lambda (x) x) 
                                                   (map (lambda (e) (if (and (list? e) 
                                                                             (equal? (first e) 'lambda)
                                                                             (= (length (second e)) 1))
                                                                        e
                                                                        #f))
                                                        (set->list fv))))
                              (map (lambda (lam)
                                           `(,(third lam) 
                                             ,(store-join sigma (hash (hash-ref saved (first (second lam))) (delta op aevs)))))
                                   clos)]
                             [`(set!/k ,xl ,ae ,aek)
                              (define fv (atomic aek sigma)) 
                              (define aev (atomic aek sigma)) 
                              (define clos (filter (lambda (x) x) 
                                                   (map (lambda (e) (if (and (list? e) 
                                                                             (equal? (first e) 'lambda)
                                                                             (= (length (second e)) 1))
                                                                        e
                                                                        #f))
                                                        (set->list fv))))
                              (map (lambda (lam)
                                           `(,(third lam) 
                                             ,(store-join sigma (hash (hash-ref saved xl) aev (hash-ref saved (first (second lam))) (set 'VOID)))))
                                   clos)]
                             [`(if ,ae ,et ,ef)
                              (define aev (atomic ae sigma)) 
                              (foldl (lambda (v el acc) 
                                             (if (set-member? aev v) 
                                                 (cons `(,el ,sigma) acc) 
                                                 acc)) 
                                     '() 
                                     '(TRUE FALSE) 
                                     `(,et ,ef))]
                             [`(,aef ,aes ...)
                              (define fv (atomic aef sigma)) 
                              (define aevs (map (lambda (ae) (atomic ae sigma)) aes))  
                              (define clos (filter (lambda (x) x) 
                                                   (map (lambda (e) (if (and (list? e) 
                                                                             (equal? (first e) 'lambda)
                                                                             (= (length (second e)) (length aevs)))
                                                                        e
                                                                        #f))
                                                        (set->list fv))))
                              (map (lambda (lam)
                                           `(,(third lam) 
                                             ,(store-join sigma (foldl (lambda (xl v h) (hash-set h (hash-ref saved xl) v)) (hash) (second lam) aevs))))
                                   clos)])]))


(define (explore reachable sigma)
        ;(pretty-print `(explore ,reachable sigma))
        (let* ([updated (foldl (lambda (inc acc) `(,(set-union (first inc) (first acc)) ,(store-join (second inc) (second acc))))
                               `(,reachable ,sigma)
                               (map (lambda (next)
                                            (let* ([succs (successors `(,next ,sigma))]
                                                   [reachable+ (list->set (foldl (lambda (succ r+) (cons (first succ) r+)) '() succs))]
                                                   [sigma+ (foldl store-join sigma (foldl (lambda (succ stores) (cons (second succ) stores)) '() succs))])
                                                  `(,reachable+ ,sigma+)))
                                    (set->list reachable)))]
               [sigma+ (second updated)]
               [reachable+ (first updated)])
              (if (and (equal? reachable reachable+) (equal? sigma sigma+))
                  sigma
                  (explore reachable+ sigma+))))


(define sigma (explore (set root) (hash)))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(display "Labels: ")
(pretty-print saved)
(newline)

(define msigma (hash))

(define (matrix-store in)
  (define row (read in))
  (when (not (eof-object? row))
    (define col (read in))
    
    ; should only print if row < (length X*T)
    (when (< row lenX)
      (define x (list-ref X row))
      (define value (list-ref V col))
      
      (set! msigma (store-join msigma (hash x (set (if (number? value) (hash-ref saved value) value))))))
    
    (matrix-store in)))

; read the matrix name and number of rows and columns
(define _ (read store-matrix))
(define store-num-rows (read store-matrix))
(define store-num-cols (read store-matrix))
(matrix-store store-matrix)



; Compare the two
(display "Differences listed below: ")
(newline)
(void (map (lambda (k) (when (not (equal? (hash-ref msigma k (lambda () (set))) (hash-ref sigma k (lambda () (set))))) 
                             (pretty-print `(cpu-sigma ,k ,(hash-ref sigma k (lambda () (set)))))
                             (pretty-print `(gpu-sigma ,k ,(hash-ref msigma k (lambda () (set)))))
                             (newline)))
           (set->list (set-union (list->set (hash-keys msigma)) (list->set (hash-keys sigma))))))


