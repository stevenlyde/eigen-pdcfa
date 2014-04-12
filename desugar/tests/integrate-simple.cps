(prim
 void
 (lambda (myfold)
   ((lambda (nn91)
      ((lambda (nn96)
         (set!/k
          myfold
          nn96
          (lambda (na97)
            (prim void (lambda (nn98) (nn91 nn98 (lambda (v) (halt))))))))
       (lambda (foldf acc lst lamk104)
         (prim
          null?
          lst
          (lambda (nn92)
            (if nn92
              (lamk104 acc)
              (prim
               car
               lst
               (lambda (nn93)
                 (foldf
                  nn93
                  acc
                  (lambda (nn94)
                    (prim
                     cdr
                     lst
                     (lambda (nn95)
                       (myfold foldf nn94 nn95 lamk104)))))))))))))
    (lambda (na39 lamk109)
      (prim
       void
       (lambda (integrate)
         ((lambda (nn72)
            ((lambda (nn88)
               (set!/k
                integrate
                nn88
                (lambda (na89)
                  (prim void (lambda (nn90) (nn72 nn90 lamk109))))))
             (lambda (fun rule start stop delta lamk115)
               (prim
                void
                (lambda (generatepoints)
                  ((lambda (nn79)
                     ((lambda (nn85)
                        (set!/k
                         generatepoints
                         nn85
                         (lambda (na86)
                           (prim void (lambda (nn87) (nn79 nn87 lamk115))))))
                      (lambda (pointslst lamk121)
                        (prim
                         car
                         pointslst
                         (lambda (nn80)
                           (prim
                            car
                            nn80
                            (lambda (last)
                              (prim
                               <
                               last
                               start
                               (lambda (nn81)
                                 (if nn81
                                   (prim cdr pointslst lamk121)
                                   (prim
                                    -
                                    last
                                    delta
                                    (lambda (nn82)
                                      (prim
                                       cons
                                       nn82
                                       last
                                       (lambda (nn83)
                                         (prim
                                          cons
                                          nn83
                                          pointslst
                                          (lambda (nn84)
                                            (generatepoints
                                             nn84
                                             lamk121)))))))))))))))))
                   (lambda (na44 lamk128)
                     ((lambda (nn74)
                        (prim
                         -
                         stop
                         delta
                         (lambda (nn75)
                           (prim
                            cons
                            nn75
                            stop
                            (lambda (nn76)
                              (prim
                               list
                               nn76
                               (lambda (nn77)
                                 (generatepoints
                                  nn77
                                  (lambda (nn78)
                                    (myfold nn74 0 nn78 lamk128))))))))))
                      (lambda (pnts sum lamk134)
                        (rule
                         fun
                         pnts
                         (lambda (nn73) (prim + sum nn73 lamk134))))))))))))
          (lambda (na40 lamk136)
            (prim
             void
             (lambda (midpoint_rule)
               ((lambda (nn60)
                  ((lambda (nn69)
                     (set!/k
                      midpoint_rule
                      nn69
                      (lambda (na70)
                        (prim void (lambda (nn71) (nn60 nn71 lamk136))))))
                   (lambda (func points lamk142)
                     (prim
                      cdr
                      points
                      (lambda (nn61)
                        (prim
                         car
                         points
                         (lambda (nn62)
                           (prim
                            -
                            nn61
                            nn62
                            (lambda (nn63)
                              (prim
                               car
                               points
                               (lambda (nn64)
                                 (prim
                                  cdr
                                  points
                                  (lambda (nn65)
                                    (prim
                                     +
                                     nn64
                                     nn65
                                     (lambda (nn66)
                                       (prim
                                        /
                                        nn66
                                        2.0
                                        (lambda (nn67)
                                          (func
                                           nn67
                                           (lambda (nn68)
                                             (prim
                                              *
                                              nn63
                                              nn68
                                              lamk142))))))))))))))))))))
                (lambda (na41 lamk151)
                  (prim
                   void
                   (lambda (rules)
                     ((lambda (nn56)
                        (prim
                         list
                         midpoint_rule
                         (lambda (nn57)
                           (set!/k
                            rules
                            nn57
                            (lambda (na58)
                              (prim
                               void
                               (lambda (nn59) (nn56 nn59 lamk151))))))))
                      (lambda (na42 lamk157)
                        (prim
                         void
                         (lambda (funcs)
                           ((lambda (nn49)
                              ((lambda (nn50)
                                 ((lambda (nn51)
                                    ((lambda (nn52)
                                       (prim
                                        list
                                        nn50
                                        nn51
                                        nn52
                                        (lambda (nn53)
                                          (set!/k
                                           funcs
                                           nn53
                                           (lambda (na54)
                                             (prim
                                              void
                                              (lambda (nn55)
                                                (nn49 nn55 lamk157))))))))
                                     (lambda (x lamk166)
                                       (prim * x x x lamk166))))
                                  (lambda (x lamk167) (prim * x x lamk167))))
                               (lambda (x lamk168) (prim sqrt x lamk168))))
                            (lambda (na43 lamk169)
                              ((lambda (nn47)
                                 (prim
                                  void
                                  (lambda (nn48)
                                    (myfold nn47 nn48 rules lamk169))))
                               (lambda (rule na lamk172)
                                 ((lambda (nn46)
                                    (myfold nn46 na funcs lamk172))
                                  (lambda (afunc na lamk174)
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
                                        lamk174)))))))))))))))))))))))))))

