(prim
 void
 (lambda (myfold)
   ((lambda (nn89)
      ((lambda (nn94)
         (set!/k
          myfold
          nn94
          (lambda (na95)
            (prim void (lambda (nn96) (nn89 nn96 (lambda (v) (halt))))))))
       (lambda (foldf acc lst lamk102)
         (prim
          null?
          lst
          (lambda (nn90)
            (if nn90
              (lamk102 acc)
              (prim
               car
               lst
               (lambda (nn91)
                 (foldf
                  nn91
                  acc
                  (lambda (nn92)
                    (prim
                     cdr
                     lst
                     (lambda (nn93)
                       (myfold foldf nn92 nn93 lamk102)))))))))))))
    (lambda (na39 lamk107)
      (prim
       void
       (lambda (integrate)
         ((lambda (nn70)
            ((lambda (nn86)
               (set!/k
                integrate
                nn86
                (lambda (na87)
                  (prim void (lambda (nn88) (nn70 nn88 lamk107))))))
             (lambda (fun rule start stop delta lamk113)
               (prim
                void
                (lambda (generatepoints)
                  ((lambda (nn77)
                     ((lambda (nn83)
                        (set!/k
                         generatepoints
                         nn83
                         (lambda (na84)
                           (prim void (lambda (nn85) (nn77 nn85 lamk113))))))
                      (lambda (pointslst lamk119)
                        (prim
                         car
                         pointslst
                         (lambda (nn78)
                           (prim
                            car
                            nn78
                            (lambda (last)
                              (prim
                               <
                               last
                               start
                               (lambda (nn79)
                                 (if nn79
                                   (prim cdr pointslst lamk119)
                                   (prim
                                    -
                                    last
                                    delta
                                    (lambda (nn80)
                                      (prim
                                       cons
                                       nn80
                                       last
                                       (lambda (nn81)
                                         (prim
                                          cons
                                          nn81
                                          pointslst
                                          (lambda (nn82)
                                            (generatepoints
                                             nn82
                                             lamk119)))))))))))))))))
                   (lambda (na44 lamk126)
                     ((lambda (nn72)
                        (prim
                         -
                         stop
                         delta
                         (lambda (nn73)
                           (prim
                            cons
                            nn73
                            stop
                            (lambda (nn74)
                              (prim
                               list
                               nn74
                               (lambda (nn75)
                                 (generatepoints
                                  nn75
                                  (lambda (nn76)
                                    (myfold nn72 0 nn76 lamk126))))))))))
                      (lambda (pnts sum lamk132)
                        (rule
                         fun
                         pnts
                         (lambda (nn71) (prim + sum nn71 lamk132))))))))))))
          (lambda (na40 lamk134)
            (prim
             void
             (lambda (midpoint_rule)
               ((lambda (nn58)
                  ((lambda (nn67)
                     (set!/k
                      midpoint_rule
                      nn67
                      (lambda (na68)
                        (prim void (lambda (nn69) (nn58 nn69 lamk134))))))
                   (lambda (func points lamk140)
                     (prim
                      cdr
                      points
                      (lambda (nn59)
                        (prim
                         car
                         points
                         (lambda (nn60)
                           (prim
                            -
                            nn59
                            nn60
                            (lambda (nn61)
                              (prim
                               car
                               points
                               (lambda (nn62)
                                 (prim
                                  cdr
                                  points
                                  (lambda (nn63)
                                    (prim
                                     +
                                     nn62
                                     nn63
                                     (lambda (nn64)
                                       (prim
                                        /
                                        nn64
                                        2.0
                                        (lambda (nn65)
                                          (func
                                           nn65
                                           (lambda (nn66)
                                             (prim
                                              *
                                              nn61
                                              nn66
                                              lamk140))))))))))))))))))))
                (lambda (na41 lamk149)
                  (prim
                   void
                   (lambda (rules)
                     ((lambda (nn54)
                        (prim
                         list
                         midpoint_rule
                         (lambda (nn55)
                           (set!/k
                            rules
                            nn55
                            (lambda (na56)
                              (prim
                               void
                               (lambda (nn57) (nn54 nn57 lamk149))))))))
                      (lambda (na42 lamk155)
                        (prim
                         void
                         (lambda (funcs)
                           ((lambda (nn49)
                              ((lambda (nn50)
                                 (prim
                                  list
                                  nn50
                                  (lambda (nn51)
                                    (set!/k
                                     funcs
                                     nn51
                                     (lambda (na52)
                                       (prim
                                        void
                                        (lambda (nn53)
                                          (nn49 nn53 lamk155))))))))
                               (lambda (x lamk162) (prim sqrt x lamk162))))
                            (lambda (na43 lamk163)
                              ((lambda (nn47)
                                 (prim
                                  void
                                  (lambda (nn48)
                                    (myfold nn47 nn48 rules lamk163))))
                               (lambda (rule na lamk166)
                                 ((lambda (nn46)
                                    (myfold nn46 na funcs lamk166))
                                  (lambda (afunc na lamk168)
                                    (integrate
                                     afunc
                                     rule
                                     0.0
                                     2.0
                                     0.001
                                     (lambda (nn45)
                                       (prim
                                        pretty-print
                                        nn45
                                        lamk168)))))))))))))))))))))))))))

