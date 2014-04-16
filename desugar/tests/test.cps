(prim
 void
 (lambda (a0)
   ((lambda (a1)
      ((lambda (a2)
         (set!/k
          a0
          a2
          (lambda (a3)
            (prim void (lambda (a4) (a1 a4 (lambda (a5) (halt))))))))
       (lambda (a6 a7 a8 a9)
         (prim
          null?
          a8
          (lambda (a10)
            (if a10
              (a9 a7)
              (prim
               car
               a8
               (lambda (a11)
                 (a6
                  a11
                  a7
                  (lambda (a12)
                    (prim cdr a8 (lambda (a13) (a0 a6 a12 a13 a9)))))))))))))
    (lambda (a14 a15)
      (prim
       void
       (lambda (a16)
         ((lambda (a17)
            ((lambda (a18)
               ((lambda (a19)
                  (prim
                   list
                   1
                   2
                   a18
                   a19
                   3
                   (lambda (a20)
                     (set!/k
                      a16
                      a20
                      (lambda (a21)
                        (prim void (lambda (a22) (a17 a22 a15))))))))
                'b))
             'a))
          (lambda (a23 a24)
            ((lambda (a25)
               (prim
                list
                (lambda (a26)
                  (a0
                   a25
                   a26
                   a16
                   (lambda (a27) (prim pretty-print a27 a24))))))
             (lambda (a28 a29 a30)
               ((lambda (a31)
                  (prim
                   equal?
                   a28
                   a31
                   (lambda (a32)
                     (if a32
                       ((lambda (a33) (prim cons a33 a29 a30)) 'YAY)
                       (prim cons a28 a29 a30)))))
                'a)))))))))))

