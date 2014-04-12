(prim
 void
 (lambda (myfold)
   ((lambda (nn42)
      ((lambda (nn47)
         (set!/k
          myfold
          nn47
          (lambda (na48)
            (prim void (lambda (nn49) (nn42 nn49 (lambda (v) (halt))))))))
       (lambda (foldf acc lst lamk55)
         (prim
          null?
          lst
          (lambda (nn43)
            (if nn43
              (lamk55 acc)
              (prim
               car
               lst
               (lambda (nn44)
                 (foldf
                  nn44
                  acc
                  (lambda (nn45)
                    (prim
                     cdr
                     lst
                     (lambda (nn46) (myfold foldf nn45 nn46 lamk55)))))))))))))
    (lambda (na39 lamk60)
      ((lambda (nn40)
         (prim list 1 2 3 4 (lambda (nn41) (myfold nn40 0 nn41 lamk60))))
       (lambda (a b lamk63) (prim + a b lamk63)))))))

