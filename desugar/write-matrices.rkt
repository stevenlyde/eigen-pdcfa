#lang racket


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


(define root (process-input (read)))



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



;;;;;;; DEBUG
;(pretty-print (pretty-program))
;(pretty-print C)
;(pretty-print S)
;(pretty-print A)
;(pretty-print X*T)
;(pretty-print CLO)
;(pretty-print X)
;(pretty-print T)
;(pretty-print V)
;(pretty-print root)



;; Table building Utils
(define (matching-env? env env+)
                (foldl (lambda (k b)
                               (and b (equal? (hash-ref env+ k) (hash-ref env k))))
                       #t
                       (set->list (set-intersect (list->set (hash-keys env+)) (list->set (hash-keys env))))))

(define (find-clo l env)
        (define (find-clo-rem rem n)
                (if (null? rem)
                    (error `(NO-SUCH-CLOSURE ,l ,env))
                    (if (and (equal? l (first (car rem))) (matching-env? env (second (car rem))))
                        n
                        (find-clo-rem (cdr rem) (+ n 1)))))
        (find-clo-rem CLO 0))

(define (find-state l env t)
        (define (find-state-rem rem n)
                (if (null? rem)
                    (error `(NO-SUCH-STATE ,l ,env ,t))
                    (if (and (equal? l (first (car rem))) 
                             (equal? t (third (car rem))) 
                             (matching-env? env (second (car rem)))) 
                        n
                        (find-state-rem (cdr rem) (+ n 1)))))
        (find-state-rem S 0))

(define (find-c l env)
        (define (find-c-rem rem n)
                (if (null? rem)
                    (error `(NO-SUCH-C ,l ,env))
                    (if (and (equal? l (first (car rem))) 
                             (matching-env? env (second (car rem)))) 
                        n
                        (find-c-rem (cdr rem) (+ n 1)))))
        (find-c-rem C 0))

(define (ae->A ael env t)
        (define ae (hash-ref saved ael))
        (cond [(symbol? ae)
               (define xoff (- lenX (length (member ae X))))
               (define toff (hash-ref T-offs t))
               (+ (* lenT xoff) toff)]
              [(number? ae) (- lenV 1)]  ;;;;;;;;;;; this needs to change as B changes
              
              [(equal? ae #t) (- lenV 3)]  ;;;;;;;;;;; this needs to change as B changes 
              [(equal? ae #f) (- lenV 2)]  ;;;;;;;;;;; this needs to change as B changes 
              [(and (list? ae) (equal? (first ae) 'lambda))
               (define clooff (find-clo ael env))
               (+ lenX*T clooff)]))

(define (e->S el env t)
        (find-state el env t))

(define (forupto f! i)
        (if (= i 0)
            (void)
            (begin (f! i)
                   (forupto f! (- i 1)))))



;; Write the output file matrices.txt
(define out (open-output-file "matrices.txt" #:mode 'text #:exists 'replace))

;; Write table r
(display "r 1 " out)
(display lenS out)
(newline out)
(define (display-r cS remS)
        (when (not (null? remS))
              (define l (first (car remS)))
              (define e (hash-ref saved l))
              (when (equal? root l)
                    (display "0 " out)
                    (display cS out)
                    (newline out))
              (display-r (+ 1 cS) (cdr remS))))
(display-r 0 S)
(newline out)
(pretty-print 13)


;; Write table sigma
(display "sigma " out)
(display lenV out)
(display " " out)
(display lenA out)
(newline out)
(define (display-sigma [n 0])
        (if (< n lenV)
            (begin (display n out)
                   (display " " out)
                   (display (+ n lenX*T) out)
                   (newline out)
                   (display-sigma (+ n 1)))
            (void)))
(display-sigma)
(newline out)
(pretty-print 12)


;; Write table Fun
(display "Fun " out)
(display lenA out)
(display " " out)
(display lenS out)
(newline out)
(define (display-Fun cS remS)
        (when (not (null? remS))
              (define l (first (car remS)))
              (define e (hash-ref saved l))
              (define env (second (car remS)))
              (define t (third (car remS)))
              (when (and (list? e) (equal? (first e) 'set!/k))
                    (define aek (last e))
                    (define aoff (ae->A aek env t))
                    (display aoff out)
                    (display " " out)
                    (display cS out)
                    (newline out))
              (when (and (list? e) (equal? (first e) 'prim))
                    (define aek (last e))
                    (define aoff (ae->A aek env t))
                    (display aoff out)
                    (display " " out)
                    (display cS out)
                    (newline out))
              (when (not (or (not (list? e))
                             (member (first e) '(prim set!/k if lambda halt))))
                    (define aef (first e))
                    (define aoff (ae->A aef env t))
                    (display aoff out)
                    (display " " out)
                    (display cS out)
                    (newline out))
              (display-Fun (+ 1 cS) (cdr remS))))
(display-Fun 0 S)
(newline out)
(pretty-print 11)


;; Write table CondTrue
(display "CondTrue " out)
(display lenS out)
(display " " out)
(display lenS out)
(newline out)
(define (display-CondTrue cS remS)
        (when (not (null? remS))
              (define l (first (car remS)))
              (define e (hash-ref saved l))
              (define env (second (car remS)))
              (define t (third (car remS)))
              (when (and (list? e) (equal? (first e) 'if))
                    (define aet (third e))
                    (define soff (e->S aet env t))
                    (display soff out)
                    (display " " out)
                    (display cS out)
                    (newline out))
              (display-CondTrue (+ 1 cS) (cdr remS))))
(display-CondTrue 0 S)
(newline out)
(pretty-print 10)


;; Write table CondFalse
(display "CondFalse " out)
(display lenS out)
(display " " out)
(display lenS out)
(newline out)
(define (display-CondFalse cS remS)
        (when (not (null? remS))
              (define l (first (car remS)))
              (define e (hash-ref saved l))
              (define env (second (car remS)))
              (define t (third (car remS)))
              (when (and (list? e) (equal? (first e) 'if))
                    (define aef (fourth e))
                    (define soff (e->S aef env t))
                    (display soff out)
                    (display " " out)
                    (display cS out)
                    (newline out))
              (display-CondFalse (+ 1 cS) (cdr remS))))
(display-CondFalse 0 S)
(newline out)
(pretty-print 9)


;; Write table Tick
(display "Tick " out)
(display lenT out) 
(display " " out)  
(display lenS out) 
(newline out)      
(define (display-Tick cS remS)
        (when (not (null? remS))
              (define l (first (car remS)))
              (define e (hash-ref saved l))
              (define env (second (car remS)))
              (define t (third (car remS)))
              (define t+ (take (cons l t) k))
              (when (not (or (not (list? e))
                             (member (first e) '(prim set!/k if lambda halt))))
                    (define toff (hash-ref T-offs t+))
                    (display toff out)
                    (display " " out)
                    (display cS out)
                    (newline out))
              (when (and (list? e) (equal? (first e) 'set!/k))
                    (define toff (hash-ref T-offs t))
                    (display toff out)
                    (display " " out)
                    (display cS out)
                    (newline out))
              (when (and (list? e) (equal? (first e) 'prim))
                    (define toff (hash-ref T-offs t))
                    (display toff out)
                    (display " " out)
                    (display cS out)
                    (newline out))
              (display-Tick (+ 1 cS) (cdr remS))))
(display-Tick 0 S)
(newline out)
(pretty-print 8)


;; Write table Body
(display "Body " out)
(display lenC out)
(display " " out) 
(display lenV out)
(newline out)
(define (display-Body cV remV)
        (when (not (null? remV))
              (define v (car remV))
              (when (and (list? v) (hash? (second v)))
                    (define l (first v))
                    (define env (second v))
                    (define e (hash-ref saved l))
                    (define bodyl (third e))
                    (define coff (find-c bodyl env))
                    (display coff out)
                    (display " " out)
                    (display cV out)
                    (newline out))
              (display-Body (+ 1 cV) (cdr remV))))
(display-Body 0 V)
(newline out)
(pretty-print 7)


;; Write tables Arg_i
(define (write-argi i)
        (display "Arg" out)
        (display i out)
        (display " " out)
        (display lenA out)
        (display " " out)
        (display lenS out)
        (newline out)
        (define (display-Arg cS remS)
                (when (not (null? remS))
                      (define l (first (car remS)))
                      (define e (hash-ref saved l))
                      (define env (second (car remS)))
                      (define t (third (car remS)))
                      (when (and (list? e) (equal? (first e) 'set!/k) (< i 3))
                            (define ae (list-ref e i))
                            (define aoff (ae->A ae env t))
                            (display aoff out)
                            (display " " out)
                            (display cS out)
                            (newline out))
                      (when (and (not (or (not (list? e))
                                          (member (first e) '(prim set!/k if lambda halt))))
                                 (< i (length e)))
                             (define aei (list-ref e i))
                             (define aoff (ae->A aei env t))
                             (display aoff out)
                             (display " " out)
                             (display cS out)
                             (newline out))
                      (display-Arg (+ 1 cS) (cdr remS))))
        (display-Arg 0 S)
        (newline out))
(forupto write-argi (max ARGSN 2))
(pretty-print 6)


;; Write tables Var_i
(define (write-vari i)
        (display "Var" out)
        (display i out)
        (display " " out)
        (display lenX out)
        (display " " out)
        (display lenV out)
        (newline out)
        (define (display-Var cV remV)
                (when (not (null? remV))
                      (define v (car remV))
                      (when (and (list? v) (hash? (second v)))
                            (define l (first v))
                            (define e (hash-ref saved l))
                            (when (< (- i 1) (length (second e))) 
                                  (define ail (list-ref (second e) (- i 1)))
                                  (define xoff (- lenX (length (member (hash-ref saved ail) X))))
                                  (display xoff out)
                                  (display " " out)
                                  (display cV out)
                                  (newline out)))
                      (display-Var (+ 1 cV) (cdr remV))))
        (display-Var 0 V)
        (newline out))
(forupto write-vari ARGSN)
(pretty-print 5)


;; Write Tables Call_i
(define (write-calli i)
        (display "Call" out)
        (display i out)
        (display " 1 " out)
        (display lenS out)
        (newline out)
        (define (display-Call cS remS)
                (when (not (null? remS))
                      (define l (first (car remS)))
                      (define e (hash-ref saved l))
                      (when (and (not (or (not (list? e))
                                          (member (first e) '(prim set!/k if lambda halt))))
                                 (= i (- (length e) 1)))
                            (display "0 " out)
                            (display cS out)
                            (newline out))
                      (display-Call (+ 1 cS) (cdr remS))))
        (display-Call 0 S)
        (newline out))
(forupto write-calli ARGSN)
(pretty-print 4)


;; Write table If
(display "If 1 " out)
(display lenS out)
(newline out)
(define (display-If cS remS)
        (when (not (null? remS))
              (define l (first (car remS)))
              (define e (hash-ref saved l))
              (when (and (list? e) (equal? (first e) 'if))
                    (display "0 " out)
                    (display cS out)
                    (newline out))
              (display-If (+ 1 cS) (cdr remS))))
(display-If 0 S)
(newline out)
(pretty-print 3)


;; Write table Set
(display "Set 1 " out)
(display lenS out)
(newline out)
(define (display-Set cS remS)
        (when (not (null? remS))
              (define l (first (car remS)))
              (define e (hash-ref saved l))
              (when (and (list? e) (equal? (first e) 'set!/k))
                    (display "0 " out)
                    (display cS out)
                    (newline out))
              (display-Set (+ 1 cS) (cdr remS))))
(display-Set 0 S)
(newline out)
(pretty-print 2)


;; Write table PrimINT
(display "PrimInt 1 " out)
(display lenS out)
(newline out)
(define (display-PrimINT cS remS)
        (when (not (null? remS))
              (define l (first (car remS)))
              (define e (hash-ref saved l))
              (when (and (list? e) (equal? (first e) 'prim) (member (second e) '(+ - * /)))
                    (display "0 " out)
                    (display cS out)
                    (newline out))
              (display-PrimINT (+ 1 cS) (cdr remS))))
(display-PrimINT 0 S)
(newline out)
(pretty-print 1)


;; Write table PrimBOOL
(display "PrimBool 1 " out)
(display lenS out)
(newline out)
(define (display-PrimBOOL cS remS)
        (when (not (null? remS))
              (define l (first (car remS)))
              (define e (hash-ref saved l))
              (when (and (list? e) (equal? (first e) 'prim) (member (second e) '(< > <= >= = equal?)))
                    (display "0 " out)
                    (display cS out)
                    (newline out))
              (display-PrimBOOL (+ 1 cS) (cdr remS))))
(display-PrimBOOL 0 S)
(newline out)
(pretty-print 0)


;; Write table PrimVOID
(display "PrimVoid 1 " out)
(display lenS out)
(newline out)
(define (display-PrimVoid cS remS)
        (when (not (null? remS))
              (define l (first (car remS)))
              (define e (hash-ref saved l))
              (when (and (list? e) (equal? (first e) 'prim) (equal? (second e) 'void))
                    (display "0 " out)
                    (display cS out)
                    (newline out))
              (display-PrimVoid (+ 1 cS) (cdr remS))))
(display-PrimVoid 0 S)
(newline out)
(pretty-print 0)





