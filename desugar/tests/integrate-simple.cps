(prim
 void
 (lambda (myfold)
   ((lambda (nn65)
      ((lambda (nn70)
         (set!/k
          myfold
          nn70
          (lambda (na71)
            (prim void (lambda (nn72) (nn65 nn72 (lambda (v) (halt))))))))
       (lambda (foldf acc lst lamk78)
         (prim
          null?
          lst
          (lambda (nn66)
            (if nn66
              (lamk78 acc)
              (prim
               car
               lst
               (lambda (nn67)
                 (foldf
                  nn67
                  acc
                  (lambda (nn68)
                    (prim
                     cdr
                     lst
                     (lambda (nn69) (myfold foldf nn68 nn69 lamk78)))))))))))))
    (lambda (na39 lamk83)
      (prim
       void
       (lambda (integrate)
         ((lambda (nn61)
            ((lambda (nn62)
               (set!/k
                integrate
                nn62
                (lambda (na63)
                  (prim void (lambda (nn64) (nn61 nn64 lamk83))))))
             (lambda (fun rule lamk89) (rule fun 77 lamk89))))
          (lambda (na40 lamk90)
            (prim
             void
             (lambda (midpoint_rule)
               ((lambda (nn57)
                  ((lambda (nn58)
                     (set!/k
                      midpoint_rule
                      nn58
                      (lambda (na59)
                        (prim void (lambda (nn60) (nn57 nn60 lamk90))))))
                   (lambda (func points lamk96) (lamk96 0))))
                (lambda (na41 lamk97)
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
                               (lambda (nn56) (nn53 nn56 lamk97))))))))
                      (lambda (na42 lamk103)
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
                                          (nn48 nn52 lamk103))))))))
                               (lambda (x lamk110) (prim sqrt x lamk110))))
                            (lambda (na43 lamk111)
                              ((lambda (nn46)
                                 (prim
                                  void
                                  (lambda (nn47)
                                    (myfold nn46 nn47 rules lamk111))))
                               (lambda (rule na lamk114)
                                 ((lambda (nn45)
                                    (myfold nn45 na funcs lamk114))
                                  (lambda (afunc na lamk116)
                                    (integrate
                                     afunc
                                     rule
                                     (lambda (nn44)
                                       (prim
                                        pretty-print
                                        nn44
                                        lamk116)))))))))))))))))))))))))))

