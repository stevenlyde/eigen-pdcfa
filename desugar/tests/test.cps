(prim
 void
 (lambda (vect)
   ((lambda (nn58)
      (prim
       make-vector
       10
       0
       (lambda (nn59)
         (set!/k
          vect
          nn59
          (lambda (na60)
            (prim void (lambda (nn61) (nn58 nn61 (lambda (v) (halt))))))))))
    (lambda (na39 lamk67)
      (prim
       void
       (lambda (fill)
         ((lambda (nn50)
            ((lambda (nn55)
               (set!/k
                fill
                nn55
                (lambda (na56)
                  (prim void (lambda (nn57) (nn50 nn57 lamk67))))))
             (lambda (i lamk73)
               (prim
                <
                i
                0
                (lambda (nn51)
                  (if nn51
                    (prim void lamk73)
                    ((lambda (nn53)
                       (prim
                        vector-set!
                        vect
                        i
                        5
                        (lambda (nn54) (nn53 nn54 lamk73))))
                     (lambda (na43 lamk77)
                       (prim - i 1 (lambda (nn52) (fill nn52 lamk77)))))))))))
          (lambda (na40 lamk79)
            ((lambda (nn48) (fill 9 (lambda (nn49) (nn48 nn49 lamk79))))
             (lambda (na41 lamk82)
               (prim
                void
                (lambda (a)
                  ((lambda (nn44)
                     (prim
                      vector-ref
                      vect
                      3
                      (lambda (nn45)
                        (set!/k
                         a
                         nn45
                         (lambda (na46)
                           (prim void (lambda (nn47) (nn44 nn47 lamk82))))))))
                   (lambda (na42 lamk88)
                     (prim pretty-print a lamk88)))))))))))))))

