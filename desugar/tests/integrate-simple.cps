(prim
 void
 (lambda (myfold)
   ((lambda (nn74)
      ((lambda (nn79)
         (set!/k
          myfold
          nn79
          (lambda (na80)
            (prim void (lambda (nn81) (nn74 nn81 (lambda (v) (halt))))))))
       (lambda (foldf acc lst lamk87)
         (prim
          null?
          lst
          (lambda (nn75)
            (if nn75
              (lamk87 acc)
              (prim
               car
               lst
               (lambda (nn76)
                 (foldf
                  nn76
                  acc
                  (lambda (nn77)
                    (prim
                     cdr
                     lst
                     (lambda (nn78) (myfold foldf nn77 nn78 lamk87)))))))))))))
    (lambda (na39 lamk92)
      (prim
       void
       (lambda (integrate)
         ((lambda (nn66)
            ((lambda (nn71)
               (set!/k
                integrate
                nn71
                (lambda (na72)
                  (prim void (lambda (nn73) (nn66 nn73 lamk92))))))
             (lambda (fun rule start stop delta lamk98)
               ((lambda (nn68)
                  (prim
                   cons
                   start
                   stop
                   (lambda (nn69)
                     (prim
                      list
                      nn69
                      (lambda (nn70) (myfold nn68 0 nn70 lamk98))))))
                (lambda (pnts sum lamk102)
                  (rule
                   fun
                   pnts
                   (lambda (nn67) (prim + sum nn67 lamk102))))))))
          (lambda (na40 lamk104)
            (prim
             void
             (lambda (midpoint_rule)
               ((lambda (nn57)
                  ((lambda (nn63)
                     (set!/k
                      midpoint_rule
                      nn63
                      (lambda (na64)
                        (prim void (lambda (nn65) (nn57 nn65 lamk104))))))
                   (lambda (func points lamk110)
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
                                 (func
                                  nn61
                                  (lambda (nn62)
                                    (prim * nn60 nn62 lamk110))))))))))))))
                (lambda (na41 lamk116)
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
                               (lambda (nn56) (nn53 nn56 lamk116))))))))
                      (lambda (na42 lamk122)
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
                                          (nn48 nn52 lamk122))))))))
                               (lambda (x lamk129) (prim sqrt x lamk129))))
                            (lambda (na43 lamk130)
                              ((lambda (nn46)
                                 (prim
                                  void
                                  (lambda (nn47)
                                    (myfold nn46 nn47 rules lamk130))))
                               (lambda (rule na lamk133)
                                 ((lambda (nn45)
                                    (myfold nn45 na funcs lamk133))
                                  (lambda (afunc na lamk135)
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
                                        lamk135)))))))))))))))))))))))))))

