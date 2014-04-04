

(prim void (lambda (v) (set!/k v (lambda (n) (halt)) (lambda (na) (set!/k v (lambda (n2) (v 5)) (lambda (na2) (prim - 2 3 (lambda (n3) (v n3)))))))))
