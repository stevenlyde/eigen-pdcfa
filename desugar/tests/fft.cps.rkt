#lang racket

(require racket/flonum)

(define-syntax set!/k
    (syntax-rules ()
        [(_ x ae k)
         (k (set! x ae))]))


(define (prim op . aes)
        (let ([k (first (reverse aes))]
              [args (reverse (cdr (reverse aes)))])
             (k (apply op args))))
             
             
(define halt exit)
             






(prim
 void
 (lambda (four1)
   ((lambda (nn71)
      ((lambda (nn167)
         (set!/k
          four1
          nn167
          (lambda (na168)
            (prim void (lambda (nn169) (nn71 nn169 (lambda (v) (halt))))))))
       (lambda (data lamk175)
         (prim
          vector-length
          data
          (lambda (n)
            ((lambda (pi*2)
               ((lambda (nn134)
                  (prim
                   void
                   (lambda (loop1)
                     ((lambda (nn135)
                        ((lambda (nn163)
                           (set!/k
                            loop1
                            nn163
                            (lambda (na164)
                              (prim
                               void
                               (lambda (nn165)
                                 (nn135
                                  nn165
                                  (lambda (nn166) (nn134 nn166 lamk175))))))))
                         (lambda (i j lamk185)
                           (prim
                            <
                            i
                            n
                            (lambda (nn136)
                              (if nn136
                                ((lambda (nn148)
                                   (prim
                                    <
                                    i
                                    j
                                    (lambda (nn149)
                                      (if nn149
                                        ((lambda (nn157)
                                           (prim
                                            vector-ref
                                            data
                                            i
                                            (lambda (temp)
                                              ((lambda (nn158)
                                                 (prim
                                                  vector-ref
                                                  data
                                                  j
                                                  (lambda (nn159)
                                                    (prim
                                                     vector-set!
                                                     data
                                                     i
                                                     nn159
                                                     (lambda (nn160)
                                                       (nn158
                                                        nn160
                                                        (lambda (nn161)
                                                          (nn157
                                                           nn161
                                                           (lambda (nn162)
                                                             (nn148
                                                              nn162
                                                              lamk185))))))))))
                                               (lambda (na58 lamk196)
                                                 (prim
                                                  vector-set!
                                                  data
                                                  j
                                                  temp
                                                  lamk196))))))
                                         (lambda (na56 lamk197)
                                           (prim
                                            +
                                            i
                                            1
                                            (lambda (nn150)
                                              (prim
                                               vector-ref
                                               data
                                               nn150
                                               (lambda (temp)
                                                 ((lambda (nn152)
                                                    (prim
                                                     +
                                                     i
                                                     1
                                                     (lambda (nn153)
                                                       (prim
                                                        +
                                                        j
                                                        1
                                                        (lambda (nn154)
                                                          (prim
                                                           vector-ref
                                                           data
                                                           nn154
                                                           (lambda (nn155)
                                                             (prim
                                                              vector-set!
                                                              data
                                                              nn153
                                                              nn155
                                                              (lambda (nn156)
                                                                (nn152
                                                                 nn156
                                                                 lamk197))))))))))
                                                  (lambda (na57 lamk205)
                                                    (prim
                                                     +
                                                     j
                                                     1
                                                     (lambda (nn151)
                                                       (prim
                                                        vector-set!
                                                        data
                                                        nn151
                                                        temp
                                                        lamk205)))))))))))
                                        (prim
                                         void
                                         (lambda (nn162)
                                           (nn148 nn162 lamk185)))))))
                                 (lambda (na54 lamk207)
                                   (prim
                                    void
                                    (lambda (loop2)
                                      ((lambda (nn138)
                                         ((lambda (nn145)
                                            (set!/k
                                             loop2
                                             nn145
                                             (lambda (na146)
                                               (prim
                                                void
                                                (lambda (nn147)
                                                  (nn138 nn147 lamk207))))))
                                          (lambda (m j lamk213)
                                            (prim
                                             >=
                                             m
                                             2
                                             (lambda (nn139)
                                               (if nn139
                                                 (prim
                                                  >=
                                                  j
                                                  m
                                                  (lambda (nn140)
                                                    (if nn140
                                                      (prim
                                                       /
                                                       m
                                                       2
                                                       (lambda (nn141)
                                                         (prim
                                                          -
                                                          j
                                                          m
                                                          (lambda (nn142)
                                                            (loop2
                                                             nn141
                                                             nn142
                                                             lamk213)))))
                                                      (prim
                                                       +
                                                       i
                                                       2
                                                       (lambda (nn143)
                                                         (prim
                                                          +
                                                          j
                                                          m
                                                          (lambda (nn144)
                                                            (loop1
                                                             nn143
                                                             nn144
                                                             lamk213))))))))
                                                 ((lambda (nn140)
                                                    (if nn140
                                                      (prim
                                                       /
                                                       m
                                                       2
                                                       (lambda (nn141)
                                                         (prim
                                                          -
                                                          j
                                                          m
                                                          (lambda (nn142)
                                                            (loop2
                                                             nn141
                                                             nn142
                                                             lamk213)))))
                                                      (prim
                                                       +
                                                       i
                                                       2
                                                       (lambda (nn143)
                                                         (prim
                                                          +
                                                          j
                                                          m
                                                          (lambda (nn144)
                                                            (loop1
                                                             nn143
                                                             nn144
                                                             lamk213)))))))
                                                  #f)))))))
                                       (lambda (na55 lamk220)
                                         (prim
                                          /
                                          n
                                          2
                                          (lambda (nn137)
                                            (loop2 nn137 j lamk220)))))))))
                                (prim void lamk185)))))))
                      (lambda (na53 lamk222) (loop1 0 0 lamk222))))))
                (lambda (na43 lamk223)
                  (prim
                   void
                   (lambda (loop3)
                     ((lambda (nn72)
                        ((lambda (nn131)
                           (set!/k
                            loop3
                            nn131
                            (lambda (na132)
                              (prim
                               void
                               (lambda (nn133) (nn72 nn133 lamk223))))))
                         (lambda (mmax lamk229)
                           (prim
                            <
                            mmax
                            n
                            (lambda (nn73)
                              (if nn73
                                (prim
                                 exact->inexact
                                 mmax
                                 (lambda (nn74)
                                   (prim
                                    fl/
                                    pi*2
                                    nn74
                                    (lambda (theta)
                                      (prim
                                       void
                                       (lambda (x)
                                         ((lambda (nn76)
                                            (prim
                                             fl*
                                             0.5
                                             theta
                                             (lambda (nn77)
                                               (prim
                                                flsin
                                                nn77
                                                (lambda (nn78)
                                                  (set!/k
                                                   x
                                                   nn78
                                                   (lambda (na79)
                                                     (prim
                                                      void
                                                      (lambda (nn80)
                                                        (nn76
                                                         nn80
                                                         (lambda (wpr)
                                                           (prim
                                                            flsin
                                                            theta
                                                            (lambda (wpi)
                                                              ((lambda (nn82)
                                                                 (prim
                                                                  void
                                                                  (lambda (loop4)
                                                                    ((lambda (nn83)
                                                                       ((lambda (nn127)
                                                                          (set!/k
                                                                           loop4
                                                                           nn127
                                                                           (lambda (na128)
                                                                             (prim
                                                                              void
                                                                              (lambda (nn129)
                                                                                (nn83
                                                                                 nn129
                                                                                 (lambda (nn130)
                                                                                   (nn82
                                                                                    nn130
                                                                                    lamk229))))))))
                                                                        (lambda (wr
                                                                                 wi
                                                                                 m
                                                                                 lamk248)
                                                                          (prim
                                                                           <
                                                                           m
                                                                           mmax
                                                                           (lambda (nn84)
                                                                             (if nn84
                                                                               (prim
                                                                                void
                                                                                (lambda (loop5)
                                                                                  ((lambda (nn85)
                                                                                     ((lambda (nn124)
                                                                                        (set!/k
                                                                                         loop5
                                                                                         nn124
                                                                                         (lambda (na125)
                                                                                           (prim
                                                                                            void
                                                                                            (lambda (nn126)
                                                                                              (nn85
                                                                                               nn126
                                                                                               lamk248))))))
                                                                                      (lambda (i
                                                                                               lamk255)
                                                                                        (prim
                                                                                         <
                                                                                         i
                                                                                         n
                                                                                         (lambda (nn86)
                                                                                           (if nn86
                                                                                             (prim
                                                                                              +
                                                                                              i
                                                                                              mmax
                                                                                              (lambda (j)
                                                                                                (prim
                                                                                                 vector-ref
                                                                                                 data
                                                                                                 j
                                                                                                 (lambda (nn87)
                                                                                                   (prim
                                                                                                    fl*
                                                                                                    wr
                                                                                                    nn87
                                                                                                    (lambda (nn88)
                                                                                                      (prim
                                                                                                       +
                                                                                                       j
                                                                                                       1
                                                                                                       (lambda (nn89)
                                                                                                         (prim
                                                                                                          vector-ref
                                                                                                          data
                                                                                                          nn89
                                                                                                          (lambda (nn90)
                                                                                                            (prim
                                                                                                             fl*
                                                                                                             wi
                                                                                                             nn90
                                                                                                             (lambda (nn91)
                                                                                                               (prim
                                                                                                                fl-
                                                                                                                nn88
                                                                                                                nn91
                                                                                                                (lambda (tempr)
                                                                                                                  (prim
                                                                                                                   +
                                                                                                                   j
                                                                                                                   1
                                                                                                                   (lambda (nn92)
                                                                                                                     (prim
                                                                                                                      vector-ref
                                                                                                                      data
                                                                                                                      nn92
                                                                                                                      (lambda (nn93)
                                                                                                                        (prim
                                                                                                                         fl*
                                                                                                                         wr
                                                                                                                         nn93
                                                                                                                         (lambda (nn94)
                                                                                                                           (prim
                                                                                                                            vector-ref
                                                                                                                            data
                                                                                                                            j
                                                                                                                            (lambda (nn95)
                                                                                                                              (prim
                                                                                                                               fl*
                                                                                                                               wi
                                                                                                                               nn95
                                                                                                                               (lambda (nn96)
                                                                                                                                 (prim
                                                                                                                                  fl+
                                                                                                                                  nn94
                                                                                                                                  nn96
                                                                                                                                  (lambda (tempi)
                                                                                                                                    ((lambda (nn98)
                                                                                                                                       (prim
                                                                                                                                        vector-ref
                                                                                                                                        data
                                                                                                                                        i
                                                                                                                                        (lambda (nn99)
                                                                                                                                          (prim
                                                                                                                                           fl-
                                                                                                                                           nn99
                                                                                                                                           tempr
                                                                                                                                           (lambda (nn100)
                                                                                                                                             (prim
                                                                                                                                              vector-set!
                                                                                                                                              data
                                                                                                                                              j
                                                                                                                                              nn100
                                                                                                                                              (lambda (nn101)
                                                                                                                                                (prim
                                                                                                                                                 +
                                                                                                                                                 j
                                                                                                                                                 1
                                                                                                                                                 (lambda (nn102)
                                                                                                                                                   (prim
                                                                                                                                                    +
                                                                                                                                                    i
                                                                                                                                                    1
                                                                                                                                                    (lambda (nn103)
                                                                                                                                                      (prim
                                                                                                                                                       vector-ref
                                                                                                                                                       data
                                                                                                                                                       nn103
                                                                                                                                                       (lambda (nn104)
                                                                                                                                                         (prim
                                                                                                                                                          fl-
                                                                                                                                                          nn104
                                                                                                                                                          tempi
                                                                                                                                                          (lambda (nn105)
                                                                                                                                                            (prim
                                                                                                                                                             vector-set!
                                                                                                                                                             data
                                                                                                                                                             nn102
                                                                                                                                                             nn105
                                                                                                                                                             (lambda (nn106)
                                                                                                                                                               (prim
                                                                                                                                                                vector-ref
                                                                                                                                                                data
                                                                                                                                                                i
                                                                                                                                                                (lambda (nn107)
                                                                                                                                                                  (prim
                                                                                                                                                                   fl+
                                                                                                                                                                   nn107
                                                                                                                                                                   tempr
                                                                                                                                                                   (lambda (nn108)
                                                                                                                                                                     (prim
                                                                                                                                                                      vector-set!
                                                                                                                                                                      data
                                                                                                                                                                      i
                                                                                                                                                                      nn108
                                                                                                                                                                      (lambda (nn109)
                                                                                                                                                                        (prim
                                                                                                                                                                         +
                                                                                                                                                                         i
                                                                                                                                                                         1
                                                                                                                                                                         (lambda (nn110)
                                                                                                                                                                           (prim
                                                                                                                                                                            +
                                                                                                                                                                            i
                                                                                                                                                                            1
                                                                                                                                                                            (lambda (nn111)
                                                                                                                                                                              (prim
                                                                                                                                                                               vector-ref
                                                                                                                                                                               data
                                                                                                                                                                               nn111
                                                                                                                                                                               (lambda (nn112)
                                                                                                                                                                                 (prim
                                                                                                                                                                                  fl+
                                                                                                                                                                                  nn112
                                                                                                                                                                                  tempi
                                                                                                                                                                                  (lambda (nn113)
                                                                                                                                                                                    (prim
                                                                                                                                                                                     vector-set!
                                                                                                                                                                                     data
                                                                                                                                                                                     nn110
                                                                                                                                                                                     nn113
                                                                                                                                                                                     (lambda (nn114)
                                                                                                                                                                                       (nn98
                                                                                                                                                                                        nn101
                                                                                                                                                                                        nn106
                                                                                                                                                                                        nn109
                                                                                                                                                                                        nn114
                                                                                                                                                                                        lamk255))))))))))))))))))))))))))))))))))
                                                                                                                                     (lambda (na49
                                                                                                                                              na50
                                                                                                                                              na51
                                                                                                                                              na52
                                                                                                                                              lamk287)
                                                                                                                                       (prim
                                                                                                                                        +
                                                                                                                                        j
                                                                                                                                        mmax
                                                                                                                                        (lambda (nn97)
                                                                                                                                          (loop5
                                                                                                                                           nn97
                                                                                                                                           lamk287)))))))))))))))))))))))))))))))
                                                                                             (prim
                                                                                              fl*
                                                                                              wr
                                                                                              wpr
                                                                                              (lambda (nn115)
                                                                                                (prim
                                                                                                 fl*
                                                                                                 wi
                                                                                                 wpi
                                                                                                 (lambda (nn116)
                                                                                                   (prim
                                                                                                    fl-
                                                                                                    nn115
                                                                                                    nn116
                                                                                                    (lambda (nn117)
                                                                                                      (prim
                                                                                                       fl+
                                                                                                       nn117
                                                                                                       wr
                                                                                                       (lambda (nn118)
                                                                                                         (prim
                                                                                                          fl*
                                                                                                          wi
                                                                                                          wpr
                                                                                                          (lambda (nn119)
                                                                                                            (prim
                                                                                                             fl*
                                                                                                             wr
                                                                                                             wpi
                                                                                                             (lambda (nn120)
                                                                                                               (prim
                                                                                                                fl+
                                                                                                                nn119
                                                                                                                nn120
                                                                                                                (lambda (nn121)
                                                                                                                  (prim
                                                                                                                   fl+
                                                                                                                   nn121
                                                                                                                   wi
                                                                                                                   (lambda (nn122)
                                                                                                                     (prim
                                                                                                                      +
                                                                                                                      m
                                                                                                                      2
                                                                                                                      (lambda (nn123)
                                                                                                                        (loop4
                                                                                                                         nn118
                                                                                                                         nn122
                                                                                                                         nn123
                                                                                                                         lamk255)))))))))))))))))))))))))
                                                                                   (lambda (na48
                                                                                            lamk298)
                                                                                     (loop5
                                                                                      m
                                                                                      lamk298)))))
                                                                               (prim
                                                                                void
                                                                                lamk248)))))))
                                                                     (lambda (na47
                                                                              lamk299)
                                                                       (loop4
                                                                        1.0
                                                                        0.0
                                                                        0
                                                                        lamk299))))))
                                                               (lambda (na46
                                                                        lamk300)
                                                                 (prim
                                                                  *
                                                                  mmax
                                                                  2
                                                                  (lambda (nn81)
                                                                    (loop3
                                                                     nn81
                                                                     lamk300))))))))))))))))))
                                          (lambda (na45 lamk302)
                                            (prim
                                             fl*
                                             x
                                             x
                                             (lambda (nn75)
                                               (prim
                                                fl*
                                                -2.0
                                                nn75
                                                lamk302)))))))))))
                                (prim void lamk229)))))))
                      (lambda (na44 lamk304) (loop3 2 lamk304))))))))
             6.28318530717959))))))
    (lambda (na39 lamk305)
      (prim
       void
       (lambda (data)
         ((lambda (nn67)
            (prim
             make-vector
             1024
             0.0
             (lambda (nn68)
               (set!/k
                data
                nn68
                (lambda (na69)
                  (prim void (lambda (nn70) (nn67 nn70 lamk305))))))))
          (lambda (na40 lamk311)
            (prim
             void
             (lambda (run)
               ((lambda (nn61)
                  ((lambda (nn64)
                     (set!/k
                      run
                      nn64
                      (lambda (na65)
                        (prim void (lambda (nn66) (nn61 nn66 lamk311))))))
                   (lambda (data lamk317)
                     ((lambda (nn62)
                        (four1 data (lambda (nn63) (nn62 nn63 lamk317))))
                      (lambda (na42 lamk320)
                        (prim vector-ref data 0 lamk320))))))
                (lambda (na41 lamk321)
                  (prim
                   make-vector
                   1024
                   0.5
                   (lambda (nn59)
                     (run
                      nn59
                      (lambda (nn60) (prim print nn60 lamk321)))))))))))))))))

