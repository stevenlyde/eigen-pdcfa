(prim
 void
 (lambda (myfold)
   ((lambda (nn46)
      ((lambda (nn51)
         (set!/k
          myfold
          nn51
          (lambda (na52)
            (prim void (lambda (nn53) (nn46 nn53 (lambda (v) (halt))))))))
       (lambda (foldf acc lst lamk59)
         (prim
          null?
          lst
          (lambda (nn47)
            (if nn47
              (lamk59 acc)
              (prim
               car
               lst
               (lambda (nn48)
                 (foldf
                  nn48
                  acc
                  (lambda (nn49)
                    (prim
                     cdr
                     lst
                     (lambda (nn50) (myfold foldf nn49 nn50 lamk59)))))))))))))
    (lambda (na39 lamk64)
      ((lambda (nn40)
         (quote
          1
          (lambda (nn41)
            (quote
             2
             (lambda (nn42)
               (quote
                3
                (lambda (nn43)
                  (quote
                   4
                   (lambda (nn44)
                     (prim
                      list
                      nn41
                      nn42
                      nn43
                      nn44
                      (lambda (nn45) (myfold nn40 0 nn45 lamk64))))))))))))
       (lambda (a b lamk71) (prim + a b lamk71)))))))

