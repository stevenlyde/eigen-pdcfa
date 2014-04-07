

(prim void (lambda (v) (prim < 1 2 (lambda (b) (if b (if #t (set! v #t (lambda (na4) (halt)))
                                                            (set! v #f (lambda (na1) (halt))))
                                                     (if #f (set! v #f (lambda (na2) (halt)))
                                                            (set! v 5 (lambda (na3) (halt)))))))))
