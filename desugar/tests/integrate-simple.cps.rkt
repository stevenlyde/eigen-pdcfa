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
 (lambda (myfold)
   ((lambda (nn68)
      ((lambda (nn73)
         (set!/k
          myfold
          nn73
          (lambda (na74)
            (prim void (lambda (nn75) (nn68 nn75 (lambda (v) (halt))))))))
       (lambda (foldf acc lst lamk81)
         (prim
          null?
          lst
          (lambda (nn69)
            (if nn69
              (lamk81 acc)
              (prim
               car
               lst
               (lambda (nn70)
                 (foldf
                  nn70
                  acc
                  (lambda (nn71)
                    (prim
                     cdr
                     lst
                     (lambda (nn72) (myfold foldf nn71 nn72 lamk81)))))))))))))
    (lambda (na39 lamk86)
      (prim
       void
       (lambda (midpoint_rule)
         ((lambda (nn59)
            ((lambda (nn65)
               (set!/k
                midpoint_rule
                nn65
                (lambda (na66)
                  (prim void (lambda (nn67) (nn59 nn67 lamk86))))))
             (lambda (func points lamk92)
               (prim
                cdr
                points
                (lambda (nn60)
                  (prim
                   car
                   points
                   (lambda (nn61)
                     (prim
                      -
                      nn60
                      nn61
                      (lambda (nn62)
                        (prim
                         car
                         points
                         (lambda (nn63)
                           (func
                            nn63
                            (lambda (nn64)
                              (prim * nn62 nn64 lamk92))))))))))))))
          (lambda (na40 lamk98)
            (prim
             void
             (lambda (integrate)
               ((lambda (nn51)
                  ((lambda (nn56)
                     (set!/k
                      integrate
                      nn56
                      (lambda (na57)
                        (prim void (lambda (nn58) (nn51 nn58 lamk98))))))
                   (lambda (fun start stop lamk104)
                     ((lambda (nn53)
                        (prim
                         cons
                         start
                         stop
                         (lambda (nn54)
                           (prim
                            list
                            nn54
                            (lambda (nn55) (myfold nn53 0 nn55 lamk104))))))
                      (lambda (pnts sum lamk108)
                        (midpoint_rule
                         fun
                         pnts
                         (lambda (nn52) (prim + sum nn52 lamk108))))))))
                (lambda (na41 lamk110)
                  (prim
                   void
                   (lambda (funcs)
                     ((lambda (nn46)
                        ((lambda (nn47)
                           (prim
                            list
                            nn47
                            (lambda (nn48)
                              (set!/k
                               funcs
                               nn48
                               (lambda (na49)
                                 (prim
                                  void
                                  (lambda (nn50) (nn46 nn50 lamk110))))))))
                         (lambda (x lamk117) (prim sqrt x lamk117))))
                      (lambda (na42 lamk118)
                        ((lambda (nn44)
                           (prim
                            void
                            (lambda (nn45) (myfold nn44 nn45 funcs lamk118))))
                         (lambda (afunc na lamk121)
                           (integrate
                            afunc
                            0.0
                            2.0
                            (lambda (nn43)
                              (prim
                               pretty-print
                               nn43
                               lamk121)))))))))))))))))))))

