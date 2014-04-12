(prim
 void
 (lambda (myfold)
   ((lambda (nn66)
      ((lambda (nn71)
         (set!/k
          myfold
          nn71
          (lambda (na72)
            (prim void (lambda (nn73) (nn66 nn73 (lambda (v) (halt))))))))
       (lambda (foldf acc lst lamk79)
         (prim
          null?
          lst
          (lambda (nn67)
            (if nn67
              (lamk79 acc)
              (prim
               car
               lst
               (lambda (nn68)
                 (foldf
                  nn68
                  acc
                  (lambda (nn69)
                    (prim
                     cdr
                     lst
                     (lambda (nn70) (myfold foldf nn69 nn70 lamk79)))))))))))))
    (lambda (na39 lamk84)
      (prim
       void
       (lambda (integrate)
         ((lambda (nn61)
            ((lambda (nn63)
               (set!/k
                integrate
                nn63
                (lambda (na64)
                  (prim void (lambda (nn65) (nn61 nn65 lamk84))))))
             (lambda (fun rule start stop delta lamk90)
               (prim cons start stop (lambda (nn62) (rule fun nn62 lamk90))))))
          (lambda (na40 lamk92)
            (prim
             void
             (lambda (midpoint_rule)
               ((lambda (nn57)
                  ((lambda (nn58)
                     (set!/k
                      midpoint_rule
                      nn58
                      (lambda (na59)
                        (prim void (lambda (nn60) (nn57 nn60 lamk92))))))
                   (lambda (func points lamk98) (lamk98 0))))
                (lambda (na41 lamk99)
                  (prim
                   void
                   (lambda (rules)
                     ((lambda (nn53)
                        (prim
                         list
                         midpoint_rule
                         (lambda (nn54)
                           (set!/k
                            rules
                            nn54
                            (lambda (na55)
                              (prim
                               void
                               (lambda (nn56) (nn53 nn56 lamk99))))))))
                      (lambda (na42 lamk105)
                        (prim
                         void
                         (lambda (funcs)
                           ((lambda (nn48)
                              ((lambda (nn49)
                                 (prim
                                  list
                                  nn49
                                  (lambda (nn50)
                                    (set!/k
                                     funcs
                                     nn50
                                     (lambda (na51)
                                       (prim
                                        void
                                        (lambda (nn52)
                                          (nn48 nn52 lamk105))))))))
                               (lambda (x lamk112) (prim sqrt x lamk112))))
                            (lambda (na43 lamk113)
                              ((lambda (nn46)
                                 (prim
                                  void
                                  (lambda (nn47)
                                    (myfold nn46 nn47 rules lamk113))))
                               (lambda (rule na lamk116)
                                 ((lambda (nn45)
                                    (myfold nn45 na funcs lamk116))
                                  (lambda (afunc na lamk118)
                                    (integrate
                                     afunc
                                     rule
                                     0.0
                                     2.0
                                     0.001
                                     (lambda (nn44)
                                       (prim
                                        pretty-print
                                        nn44
                                        lamk118)))))))))))))))))))))))))))

