

(define (myfold foldf acc lst)
        (if (null? lst)
            acc
            (myfold foldf (foldf (car lst) acc) (cdr lst))))


(define (midpoint_rule func points)
        (* (- (cdr points) (car points)) (func (car points))))


(define (integrate fun start stop)
        (myfold (lambda (pnts sum) (+ sum (midpoint_rule fun pnts))) 0 (list (cons start stop))))

; Some functions to integrate over
(define funcs (list (lambda (x) (sqrt x))
                    ))


(myfold (lambda (afunc na) (pretty-print (integrate afunc 0.0 2.0))) (void) funcs)


