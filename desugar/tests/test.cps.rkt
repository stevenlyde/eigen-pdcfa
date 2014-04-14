#lang racket

(require racket/flonum)

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
 (lambda (symbs)
   ((lambda (nn40)
      ((lambda (nn41)
         ((lambda (nn42)
            (prim
             list
             1
             2
             nn41
             nn42
             3
             (lambda (nn43)
               (set!/k
                symbs
                nn43
                (lambda (na44)
                  (prim
                   void
                   (lambda (nn45) (nn40 nn45 (lambda (v) (halt))))))))))
          'b))
       'a))
    (lambda (na39 lamk53) (prim pretty-print symbs lamk53)))))

