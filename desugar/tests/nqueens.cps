(prim
 void
 (lambda (trace?)
   ((lambda (nn101)
      (set!/k
       trace?
       #f
       (lambda (na102)
         (prim void (lambda (nn103) (nn101 nn103 (lambda (v) (halt))))))))
    (lambda (na39 lamk108)
      (prim
       void
       (lambda (loop)
         ((lambda (nn94)
            ((lambda (nn98)
               (set!/k
                loop
                nn98
                (lambda (na99)
                  (prim void (lambda (nn100) (nn94 nn100 lamk108))))))
             (lambda (i l lamk114)
               (prim
                =
                i
                0
                (lambda (nn95)
                  (if nn95
                    (lamk114 l)
                    (prim
                     -
                     i
                     1
                     (lambda (nn96)
                       (prim
                        cons
                        i
                        l
                        (lambda (nn97) (loop nn96 nn97 lamk114)))))))))))
          (lambda (na40 lamk118)
            (prim
             void
             (lambda (iota1)
               ((lambda (nn89)
                  ((lambda (nn91)
                     (set!/k
                      iota1
                      nn91
                      (lambda (na92)
                        (prim void (lambda (nn93) (nn89 nn93 lamk118))))))
                   (lambda (n lamk124)
                     (prim list (lambda (nn90) (loop n nn90 lamk124))))))
                (lambda (na41 lamk126)
                  (prim
                   void
                   (lambda (ok?)
                     ((lambda (nn74)
                        ((lambda (nn86)
                           (set!/k
                            ok?
                            nn86
                            (lambda (na87)
                              (prim
                               void
                               (lambda (nn88) (nn74 nn88 lamk126))))))
                         (lambda (row dist placed lamk132)
                           (prim
                            null?
                            placed
                            (lambda (nn75)
                              (if nn75
                                (lamk132 #t)
                                (prim
                                 car
                                 placed
                                 (lambda (nn76)
                                   (prim
                                    +
                                    row
                                    dist
                                    (lambda (nn77)
                                      (prim
                                       =
                                       nn76
                                       nn77
                                       (lambda (nn78)
                                         (prim
                                          not
                                          nn78
                                          (lambda (nn79)
                                            (if nn79
                                              (prim
                                               car
                                               placed
                                               (lambda (nn80)
                                                 (prim
                                                  -
                                                  row
                                                  dist
                                                  (lambda (nn81)
                                                    (prim
                                                     =
                                                     nn80
                                                     nn81
                                                     (lambda (nn82)
                                                       (prim
                                                        not
                                                        nn82
                                                        (lambda (nn83)
                                                          (if nn83
                                                            (prim
                                                             +
                                                             dist
                                                             1
                                                             (lambda (nn84)
                                                               (prim
                                                                cdr
                                                                placed
                                                                (lambda (nn85)
                                                                  (ok?
                                                                   row
                                                                   nn84
                                                                   nn85
                                                                   lamk132)))))
                                                            (lamk132
                                                             #f))))))))))
                                              (lamk132 #f))))))))))))))))
                      (lambda (na42 lamk144)
                        (prim
                         void
                         (lambda (my-try)
                           ((lambda (nn54)
                              ((lambda (nn71)
                                 (set!/k
                                  my-try
                                  nn71
                                  (lambda (na72)
                                    (prim
                                     void
                                     (lambda (nn73) (nn54 nn73 lamk144))))))
                               (lambda (x y z lamk150)
                                 (prim
                                  null?
                                  x
                                  (lambda (nn55)
                                    (if nn55
                                      (prim
                                       null?
                                       y
                                       (lambda (nn56)
                                         (if nn56
                                           ((lambda (nn57)
                                              (if trace?
                                                (prim
                                                 print
                                                 z
                                                 (lambda (nn58)
                                                   (nn57 nn58 lamk150)))
                                                (prim
                                                 void
                                                 (lambda (nn58)
                                                   (nn57 nn58 lamk150)))))
                                            (lambda (na45 lamk155)
                                              (lamk155 1)))
                                           (lamk150 0))))
                                      (prim
                                       car
                                       x
                                       (lambda (nn59)
                                         (ok?
                                          nn59
                                          1
                                          z
                                          (lambda (nn60)
                                            (if nn60
                                              (prim
                                               cdr
                                               x
                                               (lambda (nn61)
                                                 (prim
                                                  append
                                                  nn61
                                                  y
                                                  (lambda (nn62)
                                                    (prim
                                                     list
                                                     (lambda (nn63)
                                                       (prim
                                                        car
                                                        x
                                                        (lambda (nn64)
                                                          (prim
                                                           cons
                                                           nn64
                                                           z
                                                           (lambda (nn65)
                                                             (my-try
                                                              nn62
                                                              nn63
                                                              nn65
                                                              (lambda (nn66)
                                                                (prim
                                                                 cdr
                                                                 x
                                                                 (lambda (nn67)
                                                                   (prim
                                                                    car
                                                                    x
                                                                    (lambda (nn68)
                                                                      (prim
                                                                       cons
                                                                       nn68
                                                                       y
                                                                       (lambda (nn69)
                                                                         (my-try
                                                                          nn67
                                                                          nn69
                                                                          z
                                                                          (lambda (nn70)
                                                                            (prim
                                                                             +
                                                                             nn66
                                                                             nn70
                                                                             lamk150)))))))))))))))))))))
                                              ((lambda (nn66)
                                                 (prim
                                                  cdr
                                                  x
                                                  (lambda (nn67)
                                                    (prim
                                                     car
                                                     x
                                                     (lambda (nn68)
                                                       (prim
                                                        cons
                                                        nn68
                                                        y
                                                        (lambda (nn69)
                                                          (my-try
                                                           nn67
                                                           nn69
                                                           z
                                                           (lambda (nn70)
                                                             (prim
                                                              +
                                                              nn66
                                                              nn70
                                                              lamk150))))))))))
                                               0))))))))))))
                            (lambda (na43 lamk168)
                              (prim
                               void
                               (lambda (nqueens)
                                 ((lambda (nn47)
                                    ((lambda (nn51)
                                       (set!/k
                                        nqueens
                                        nn51
                                        (lambda (na52)
                                          (prim
                                           void
                                           (lambda (nn53)
                                             (nn47 nn53 lamk168))))))
                                     (lambda (n lamk174)
                                       (iota1
                                        n
                                        (lambda (nn48)
                                          (prim
                                           list
                                           (lambda (nn49)
                                             (prim
                                              list
                                              (lambda (nn50)
                                                (my-try
                                                 nn48
                                                 nn49
                                                 nn50
                                                 lamk174))))))))))
                                  (lambda (na44 lamk178)
                                    (nqueens
                                     7
                                     (lambda (nn46)
                                       (prim
                                        print
                                        nn46
                                        lamk178)))))))))))))))))))))))))))

