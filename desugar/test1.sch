

(define (f g)
        (g (set! g (g g f)) f))

(define m (f (lambda (h k) ((lambda (h k) k) k h))))



