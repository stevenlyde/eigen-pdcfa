#lang racket


(define-syntax set!/k
    (syntax-rules ()
        [(_ x ae k)
         (k (set! x ae))]))


(define (prim op . aes)
        (let ([k (first (reverse aes))]
              [args (reverse (cdr (reverse aes)))])
             (k (apply op args))))
             
             
(define halt exit)
             






(prim
 void
 (lambda (a0)
   ((lambda (a1)
      ((lambda (a2)
         (set!/k
          a0
          a2
          (lambda (a3)
            (prim void (lambda (a4) (a1 a4 (lambda (a5) (halt))))))))
       (lambda (a6 a7)
         (prim
          <
          a6
          2
          (lambda (a8)
            (if a8
              (a7 a6)
              (prim
               -
               a6
               1
               (lambda (a9)
                 (a0
                  a9
                  (lambda (a10)
                    (prim
                     -
                     a6
                     2
                     (lambda (a11)
                       (a0 a11 (lambda (a12) (prim + a10 a12 a7)))))))))))))))
    (lambda (a13 a14) (a0 20 (lambda (a15) (prim print a15 a14)))))))

