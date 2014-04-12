(prim
 void
 (lambda (myfold)
   ((lambda (nn77)
      ((lambda (nn82)
         (set!/k
          myfold
          nn82
          (lambda (na83)
            (prim void (lambda (nn84) (nn77 nn84 (lambda (v) (halt))))))))
       (lambda (foldf acc lst lamk90)
         (prim
          null?
          lst
          (lambda (nn78)
            (if nn78
              (lamk90 acc)
              (prim
               car
               lst
               (lambda (nn79)
                 (foldf
                  nn79
                  acc
                  (lambda (nn80)
                    (prim
                     cdr
                     lst
                     (lambda (nn81) (myfold foldf nn80 nn81 lamk90)))))))))))))
    (lambda (na39 lamk95)
      (prim
       void
       (lambda (integrate)
         ((lambda (nn69)
            ((lambda (nn74)
               (set!/k
                integrate
                nn74
                (lambda (na75)
                  (prim void (lambda (nn76) (nn69 nn76 lamk95))))))
             (lambda (fun rule start stop delta lamk101)
               ((lambda (nn71)
                  (prim
                   cons
                   start
                   stop
                   (lambda (nn72)
                     (prim
                      list
                      nn72
                      (lambda (nn73) (myfold nn71 0 nn73 lamk101))))))
                (lambda (pnts sum lamk105)
                  (rule
                   fun
                   pnts
                   (lambda (nn70) (prim + sum nn70 lamk105))))))))
          (lambda (na40 lamk107)
            (prim
             void
             (lambda (midpoint_rule)
               ((lambda (nn57)
                  ((lambda (nn66)
                     (set!/k
                      midpoint_rule
                      nn66
                      (lambda (na67)
                        (prim void (lambda (nn68) (nn57 nn68 lamk107))))))
                   (lambda (func points lamk113)
                     (prim
                      cdr
                      points
                      (lambda (nn58)
                        (prim
                         car
                         points
                         (lambda (nn59)
                           (prim
                            -
                            nn58
                            nn59
                            (lambda (nn60)
                              (prim
                               car
                               points
                               (lambda (nn61)
                                 (prim
                                  cdr
                                  points
                                  (lambda (nn62)
                                    (prim
                                     +
                                     nn61
                                     nn62
                                     (lambda (nn63)
                                       (prim
                                        /
                                        nn63
                                        2.0
                                        (lambda (nn64)
                                          (func
                                           nn64
                                           (lambda (nn65)
                                             (prim
                                              *
                                              nn60
                                              nn65
                                              lamk113))))))))))))))))))))
                (lambda (na41 lamk122)
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
                               (lambda (nn56) (nn53 nn56 lamk122))))))))
                      (lambda (na42 lamk128)
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
                                          (nn48 nn52 lamk128))))))))
                               (lambda (x lamk135) (prim sqrt x lamk135))))
                            (lambda (na43 lamk136)
                              ((lambda (nn46)
                                 (prim
                                  void
                                  (lambda (nn47)
                                    (myfold nn46 nn47 rules lamk136))))
                               (lambda (rule na lamk139)
                                 ((lambda (nn45)
                                    (myfold nn45 na funcs lamk139))
                                  (lambda (afunc na lamk141)
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
                                        lamk141)))))))))))))))))))))))))))

