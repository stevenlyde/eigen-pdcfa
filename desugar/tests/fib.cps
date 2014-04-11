(prim
 void
 (lambda (fib)
   ((lambda (nn41)
      ((lambda (nn47)
         (set!/k
          fib
          nn47
          (lambda (na48)
            (prim void (lambda (nn49) (nn41 nn49 (lambda (v) (halt))))))))
       (lambda (n lamk55)
         (prim
          <
          n
          2
          (lambda (nn42)
            (if nn42
              (lamk55 n)
              (prim
               -
               n
               1
               (lambda (nn43)
                 (fib
                  nn43
                  (lambda (nn44)
                    (prim
                     -
                     n
                     2
                     (lambda (nn45)
                       (fib
                        nn45
                        (lambda (nn46) (prim + nn44 nn46 lamk55)))))))))))))))
    (lambda (na39 lamk61) (fib 20 (lambda (nn40) (prim print nn40 lamk61)))))))

