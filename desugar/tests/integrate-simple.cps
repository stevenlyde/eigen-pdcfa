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
       (lambda (integrate)
         ((lambda (nn60)
            ((lambda (nn65)
               (set!/k
                integrate
                nn65
                (lambda (na66)
                  (prim void (lambda (nn67) (nn60 nn67 lamk86))))))
             (lambda (fun rule start stop delta lamk92)
               ((lambda (nn62)
                  (prim
                   cons
                   start
                   stop
                   (lambda (nn63)
                     (prim
                      list
                      nn63
                      (lambda (nn64) (myfold nn62 0 nn64 lamk92))))))
                (lambda (pnts sum lamk96)
                  (rule fun pnts (lambda (nn61) (prim + sum nn61 lamk96))))))))
          (lambda (na40 lamk98)
            (prim
             void
             (lambda (midpoint_rule)
               ((lambda (nn51)
                  ((lambda (nn57)
                     (set!/k
                      midpoint_rule
                      nn57
                      (lambda (na58)
                        (prim void (lambda (nn59) (nn51 nn59 lamk98))))))
                   (lambda (func points lamk104)
                     (prim
                      cdr
                      points
                      (lambda (nn52)
                        (prim
                         car
                         points
                         (lambda (nn53)
                           (prim
                            -
                            nn52
                            nn53
                            (lambda (nn54)
                              (prim
                               car
                               points
                               (lambda (nn55)
                                 (func
                                  nn55
                                  (lambda (nn56)
                                    (prim * nn54 nn56 lamk104))))))))))))))
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
                            midpoint_rule
                            0.0
                            2.0
                            0.001
                            (lambda (nn43)
                              (prim
                               pretty-print
                               nn43
                               lamk121)))))))))))))))))))))

