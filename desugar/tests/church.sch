

(define (mygensym s) s)


(define (number->church n)
        (letrec ([aux (lambda (n) (if (= n 0) 'v (list 'f (aux (- n 1)))))])
                (list 'lambda (list 'f) (list 'lambda (list 'v) (aux n)))))
(define prim+1 (list 'lambda (list 'n) (list 'lambda (list 'f) (list 'lambda (list 'v) (list 'f (list (list 'n 'f) 'v))))))
(define prim+ (list 'lambda (list 'n) (list 'lambda (list 'm) (list 'lambda (list 'f) (list 'lambda (list 'v) 
                            (list (list 'n 'f) (list (list 'm 'f) 'v)))))))
(define prim* (list 'lambda (list 'n) (list 'lambda (list 'm) (list 'lambda (list 'f) (list 'lambda (list 'v) 
                            (list (list 'n (list 'm 'f)) 'v))))))


; A-Normalizes; adapted from Flanagan et al.
(define (anf-convert e)
        (let ((normalize-name 0) (normalize-name* 0) (normalize 0)) (begin
        (set! normalize-name (lambda (e k)
                (normalize e (lambda (norm) (if (list? norm)
                                                (let ([t (mygensym 'nn)])
                                                     (list 'let (list (list t norm)) (k t)))
                                                (k norm))))))
        (set! normalize-name* (lambda (e* k)
                (if (null? e*)
                    (k (list))
                    (normalize-name (car e*)
                                    (lambda (t) (normalize-name* (cdr e*)
                                                                 (lambda (t*) (k (cons t t*)))))))))
        (set! normalize (lambda (e k)
                 (cond [(and (list? e) (equal? (car e) 'lambda))
                        (k (list 'lambda (car (cdr e)) (anf-convert (car (cdr (cdr e))))))]
                       
                       [(and (list? e) (equal? (car e) 'let))
                        (normalize (car (cdr (car (car (cdr e)))))
                                   (lambda (norm)
                                           (list 'let (list (list (car (car (car (cdr e)))) norm)) 
                                                 (normalize (car (cdr (cdr e))) k))))]
                       
                       [(and (list? e) (equal? (car e) 'if))
                        (normalize-name (car (cdr e))
                                        (lambda (name) 
                                                (k (list 'if name 
                                                         (anf-convert (car (cdr (cdr e)))) 
                                                         (anf-convert (car (cdr (cdr (cdr e)))))))))]
                       
                       [(and (list? e) (equal? (car e) 'prim))
                        (normalize-name* (cdr (cdr e))
                                         (lambda (name*)
                                                 (k (cons 'prim (cons (car (cdr e)) name*)))))]
                       
                       [(list? e)
                        (normalize-name (car e)
                                        (lambda (name) 
                                                (normalize-name* (cdr e) 
                                                                 (lambda (name*)
                                                                         (k (cons name name*))))))]
                       
                       [else (k e)])))
        (normalize e (lambda (x) x)))))



(define (anf-convert2 e)
        (let ((normalize-name 0) (normalize-name* 0) (normalize 0)) (begin
        (set! normalize-name (lambda (e k)
                (normalize e (lambda (norm) (if (list? norm)
                                                (let ([t (mygensym 'nn)])
                                                     (list 'let (list (list t norm)) (k t)))
                                                (k norm))))))
        (set! normalize-name* (lambda (e* k)
                (if (null? e*)
                    (k (list))
                    (normalize-name (car e*)
                                    (lambda (t) (normalize-name* (cdr e*)
                                                                 (lambda (t*) (k (cons t t*)))))))))
        (set! normalize (lambda (e k)
                 (cond [(and (list? e) (equal? (car e) 'lambda))
                        (k (list 'lambda (car (cdr e)) (anf-convert (car (cdr (cdr e))))))]
                       
                       [(and (list? e) (equal? (car e) 'let))
                        (normalize (car (cdr (car (car (cdr e)))))
                                   (lambda (norm)
                                           (list 'let (list (list (car (car (car (cdr e)))) norm)) 
                                                 (normalize (car (cdr (cdr e))) k))))]
                       
                       [(and (list? e) (equal? (car e) 'if))
                        (normalize-name (car (cdr e))
                                        (lambda (name) 
                                                (k (list 'if name 
                                                         (anf-convert (car (cdr (cdr e)))) 
                                                         (anf-convert (car (cdr (cdr (cdr e)))))))))]
                       
                       [(and (list? e) (equal? (car e) 'prim))
                        (normalize-name* (cdr (cdr e))
                                         (lambda (name*)
                                                 (k (cons 'prim (cons (car (cdr e)) name*)))))]
                       
                       [(list? e)
                        (normalize-name (car e)
                                        (lambda (name) 
                                                (normalize-name* (cdr e) 
                                                                 (lambda (name*)
                                                                         (k (cons name name*))))))]
                       
                       [else (k e)])))
        (normalize e (lambda (x) x)))))



(define (anf-convert3 e)
        (let ((normalize-name 0) (normalize-name* 0) (normalize 0)) (begin
        (set! normalize-name (lambda (e k)
                (normalize e (lambda (norm) (if (list? norm)
                                                (let ([t (mygensym 'nn)])
                                                     (list 'let (list (list t norm)) (k t)))
                                                (k norm))))))
        (set! normalize-name* (lambda (e* k)
                (if (null? e*)
                    (k (list))
                    (normalize-name (car e*)
                                    (lambda (t) (normalize-name* (cdr e*)
                                                                 (lambda (t*) (k (cons t t*)))))))))
        (set! normalize (lambda (e k)
                 (cond [(and (list? e) (equal? (car e) 'lambda))
                        (k (list 'lambda (car (cdr e)) (anf-convert (car (cdr (cdr e))))))]
                       
                       [(and (list? e) (equal? (car e) 'let))
                        (normalize (car (cdr (car (car (cdr e)))))
                                   (lambda (norm)
                                           (list 'let (list (list (car (car (car (cdr e)))) norm)) 
                                                 (normalize (car (cdr (cdr e))) k))))]
                       
                       [(and (list? e) (equal? (car e) 'if))
                        (normalize-name (car (cdr e))
                                        (lambda (name) 
                                                (k (list 'if name 
                                                         (anf-convert (car (cdr (cdr e)))) 
                                                         (anf-convert (car (cdr (cdr (cdr e)))))))))]
                       
                       [(and (list? e) (equal? (car e) 'prim))
                        (normalize-name* (cdr (cdr e))
                                         (lambda (name*)
                                                 (k (cons 'prim (cons (car (cdr e)) name*)))))]
                       
                       [(list? e)
                        (normalize-name (car e)
                                        (lambda (name) 
                                                (normalize-name* (cdr e) 
                                                                 (lambda (name*)
                                                                         (k (cons name name*))))))]
                       
                       [else (k e)])))
        (normalize e (lambda (x) x)))))



(define (replace-me e pred? op)
        (cond [(null? e) e]
              [(list? e) (cons (replace-me (car e) pred? op) (replace-me (cdr e) pred? op))]
              [(pred? e) (op e)]
              [else e]))

(define (church-encode e)
        (replace-me (replace-me (replace-me e (lambda (e) (number? e)) number->church) (lambda (e) (equal? '* e)) (lambda (a) prim*)) (lambda (e) (equal? '+ e)) (lambda (a) prim+)))
(define (church-encode2 e)
        (replace-me (replace-me (replace-me e (lambda (e) (number? e)) number->church) (lambda (e) (equal? '* e)) (lambda (a) prim*)) (lambda (e) (equal? '+ e)) (lambda (a) prim+)))
(define (church-encode3 e)
        (replace-me (replace-me (replace-me e (lambda (e) (number? e)) number->church) (lambda (e) (equal? '* e)) (lambda (a) prim*)) (lambda (e) (equal? '+ e)) (lambda (a) prim+)))


(define run 0)
(set! run (lambda () (anf-convert '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (anf-convert '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (anf-convert '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (anf-convert '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (anf-convert '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (anf-convert '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (anf-convert '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (anf-convert '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (anf-convert '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode '(+ (* 5 6) (* 3 2)))))


(set! run (lambda () (anf-convert2 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode2 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (anf-convert2 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode2 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (anf-convert2 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode2 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (anf-convert2 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode2 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (anf-convert2 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode2 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode2 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (anf-convert2 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode2 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (anf-convert2 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode2 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (anf-convert2 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode2 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (anf-convert2 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode2 '(+ (* 5 6) (* 3 2)))))


(set! run (lambda () (anf-convert3 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode3 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (anf-convert3 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode3 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (anf-convert3 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode3 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (anf-convert3 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode3 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (anf-convert3 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode3 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (anf-convert3 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode3 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (anf-convert3 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode3 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (anf-convert3 '(+ (* 5 6) (* 3 2)))))
(set! run (lambda () (church-encode3 '(+ (* 5 6) (* 3 2)))))

(pretty-print (run))




