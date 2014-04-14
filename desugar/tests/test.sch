


(define vect (make-vector 10 0))

(define (fill i)
        (if (< i 0)
            (void)
            (begin (vector-set! vect i 5)
                   (fill (- i 1)))))
                   
(fill 9)

(define a (vector-ref vect 3))

(pretty-print a)
