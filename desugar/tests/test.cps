(prim
 void
 (lambda (symbs)
   ((lambda (nn40)
      ((lambda (nn41)
         ((lambda (nn42)
            (prim
             list
             1
             2
             nn41
             nn42
             3
             (lambda (nn43)
               (set!/k
                symbs
                nn43
                (lambda (na44)
                  (prim
                   void
                   (lambda (nn45) (nn40 nn45 (lambda (v) (halt))))))))))
          'b))
       'a))
    (lambda (na39 lamk53) (prim pretty-print symbs lamk53)))))

