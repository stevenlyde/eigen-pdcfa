(prim
 void
 (lambda (myfold)
   ((lambda (nn69)
      ((lambda (nn74)
         (set!/k
          myfold
          nn74
          (lambda (na75)
            (prim void (lambda (nn76) (nn69 nn76 (lambda (v) (halt))))))))
       (lambda (foldf acc lst lamk82)
         (prim
          null?
          lst
          (lambda (nn70)
            (if nn70
              (lamk82 acc)
              (prim
               car
               lst
               (lambda (nn71)
                 (foldf
                  nn71
                  acc
                  (lambda (nn72)
                    (prim
                     cdr
                     lst
                     (lambda (nn73) (myfold foldf nn72 nn73 lamk82)))))))))))))
    (lambda (na39 lamk87)
      (prim
       void
       (lambda (integrate)
         ((lambda (nn61)
            ((lambda (nn66)
               (set!/k
                integrate
                nn66
                (lambda (na67)
                  (prim void (lambda (nn68) (nn61 nn68 lamk87))))))
             (lambda (fun rule start stop delta lamk93)
               ((lambda (nn63)
                  (prim
                   cons
                   start
                   stop
                   (lambda (nn64)
                     (prim
                      list
                      nn64
                      (lambda (nn65) (myfold nn63 0 nn65 lamk93))))))
                (lambda (pnts sum lamk97)
                  (rule fun pnts (lambda (nn62) (prim + sum nn62 lamk97))))))))
          (lambda (na40 lamk99)
            (prim
             void
             (lambda (midpoint_rule)
               ((lambda (nn57)
                  ((lambda (nn58)
                     (set!/k
                      midpoint_rule
                      nn58
                      (lambda (na59)
                        (prim void (lambda (nn60) (nn57 nn60 lamk99))))))
                   (lambda (func points lamk105) (lamk105 0))))
                (lambda (na41 lamk106)
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
                               (lambda (nn56) (nn53 nn56 lamk106))))))))
                      (lambda (na42 lamk112)
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
                                          (nn48 nn52 lamk112))))))))
                               (lambda (x lamk119) (prim sqrt x lamk119))))
                            (lambda (na43 lamk120)
                              ((lambda (nn46)
                                 (prim
                                  void
                                  (lambda (nn47)
                                    (myfold nn46 nn47 rules lamk120))))
                               (lambda (rule na lamk123)
                                 ((lambda (nn45)
                                    (myfold nn45 na funcs lamk123))
                                  (lambda (afunc na lamk125)
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
                                        lamk125)))))))))))))))))))))))))))

