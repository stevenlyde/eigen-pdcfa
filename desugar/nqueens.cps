(prim
 void
 (lambda (a0)
   ((lambda (a1)
      (set!/k
       a0
       #f
       (lambda (a2) (prim void (lambda (a3) (a1 a3 (lambda (a4) (halt))))))))
    (lambda (a5 a6)
      (prim
       void
       (lambda (a7)
         ((lambda (a8)
            ((lambda (a9)
               (set!/k
                a7
                a9
                (lambda (a10) (prim void (lambda (a11) (a8 a11 a6))))))
             (lambda (a12 a13 a14)
               (prim
                =
                a12
                0
                (lambda (a15)
                  (if a15
                    (a14 a13)
                    (prim
                     -
                     a12
                     1
                     (lambda (a16)
                       (prim
                        cons
                        a12
                        a13
                        (lambda (a17) (a7 a16 a17 a14)))))))))))
          (lambda (a18 a19)
            (prim
             void
             (lambda (a20)
               ((lambda (a21)
                  ((lambda (a22)
                     (set!/k
                      a20
                      a22
                      (lambda (a23) (prim void (lambda (a24) (a21 a24 a19))))))
                   (lambda (a25 a26)
                     (prim list (lambda (a27) (a7 a25 a27 a26))))))
                (lambda (a28 a29)
                  (prim
                   void
                   (lambda (a30)
                     ((lambda (a31)
                        ((lambda (a32)
                           (set!/k
                            a30
                            a32
                            (lambda (a33)
                              (prim void (lambda (a34) (a31 a34 a29))))))
                         (lambda (a35 a36 a37 a38)
                           (prim
                            null?
                            a37
                            (lambda (a39)
                              (if a39
                                (a38 #t)
                                (prim
                                 car
                                 a37
                                 (lambda (a40)
                                   (prim
                                    +
                                    a35
                                    a36
                                    (lambda (a41)
                                      (prim
                                       =
                                       a40
                                       a41
                                       (lambda (a42)
                                         (prim
                                          not
                                          a42
                                          (lambda (a43)
                                            (if a43
                                              (prim
                                               car
                                               a37
                                               (lambda (a44)
                                                 (prim
                                                  -
                                                  a35
                                                  a36
                                                  (lambda (a45)
                                                    (prim
                                                     =
                                                     a44
                                                     a45
                                                     (lambda (a46)
                                                       (prim
                                                        not
                                                        a46
                                                        (lambda (a47)
                                                          (if a47
                                                            (prim
                                                             +
                                                             a36
                                                             1
                                                             (lambda (a48)
                                                               (prim
                                                                cdr
                                                                a37
                                                                (lambda (a49)
                                                                  (a30
                                                                   a35
                                                                   a48
                                                                   a49
                                                                   a38)))))
                                                            (a38 #f))))))))))
                                              (a38 #f))))))))))))))))
                      (lambda (a50 a51)
                        (prim
                         void
                         (lambda (a52)
                           ((lambda (a53)
                              ((lambda (a54)
                                 (set!/k
                                  a52
                                  a54
                                  (lambda (a55)
                                    (prim void (lambda (a56) (a53 a56 a51))))))
                               (lambda (a57 a58 a59 a60)
                                 (prim
                                  null?
                                  a57
                                  (lambda (a61)
                                    (if a61
                                      (prim
                                       null?
                                       a58
                                       (lambda (a62)
                                         (if a62
                                           ((lambda (a63)
                                              (if a0
                                                (prim
                                                 print
                                                 a59
                                                 (lambda (a64) (a63 a64 a60)))
                                                (prim
                                                 void
                                                 (lambda (a65)
                                                   (a63 a65 a60)))))
                                            (lambda (a66 a67) (a67 1)))
                                           (a60 0))))
                                      (prim
                                       car
                                       a57
                                       (lambda (a68)
                                         (a30
                                          a68
                                          1
                                          a59
                                          (lambda (a69)
                                            (if a69
                                              (prim
                                               cdr
                                               a57
                                               (lambda (a70)
                                                 (prim
                                                  append
                                                  a70
                                                  a58
                                                  (lambda (a71)
                                                    (prim
                                                     list
                                                     (lambda (a72)
                                                       (prim
                                                        car
                                                        a57
                                                        (lambda (a73)
                                                          (prim
                                                           cons
                                                           a73
                                                           a59
                                                           (lambda (a74)
                                                             (a52
                                                              a71
                                                              a72
                                                              a74
                                                              (lambda (a75)
                                                                (prim
                                                                 cdr
                                                                 a57
                                                                 (lambda (a76)
                                                                   (prim
                                                                    car
                                                                    a57
                                                                    (lambda (a77)
                                                                      (prim
                                                                       cons
                                                                       a77
                                                                       a58
                                                                       (lambda (a78)
                                                                         (a52
                                                                          a76
                                                                          a78
                                                                          a59
                                                                          (lambda (a79)
                                                                            (prim
                                                                             +
                                                                             a75
                                                                             a79
                                                                             a60)))))))))))))))))))))
                                              ((lambda (a80)
                                                 (prim
                                                  cdr
                                                  a57
                                                  (lambda (a81)
                                                    (prim
                                                     car
                                                     a57
                                                     (lambda (a82)
                                                       (prim
                                                        cons
                                                        a82
                                                        a58
                                                        (lambda (a83)
                                                          (a52
                                                           a81
                                                           a83
                                                           a59
                                                           (lambda (a84)
                                                             (prim
                                                              +
                                                              a80
                                                              a84
                                                              a60))))))))))
                                               0))))))))))))
                            (lambda (a85 a86)
                              (prim
                               void
                               (lambda (a87)
                                 ((lambda (a88)
                                    ((lambda (a89)
                                       (set!/k
                                        a87
                                        a89
                                        (lambda (a90)
                                          (prim
                                           void
                                           (lambda (a91) (a88 a91 a86))))))
                                     (lambda (a92 a93)
                                       (a20
                                        a92
                                        (lambda (a94)
                                          (prim
                                           list
                                           (lambda (a95)
                                             (prim
                                              list
                                              (lambda (a96)
                                                (a52 a94 a95 a96 a93))))))))))
                                  (lambda (a97 a98)
                                    (a87
                                     7
                                     (lambda (a99)
                                       (prim
                                        print
                                        a99
                                        a98)))))))))))))))))))))))))))

