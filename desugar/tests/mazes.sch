; Adapted from http://www.ccs.neu.edu/home/will/Twobit/benchmarksAboutR6.html (by Marc Feeley)


(define odd?
  (lambda (n)
    (= 1 (modulo n 2))))

(define even?
  (lambda (n)
    (= 0 (modulo n 2))))
    
(define map
  (lambda (f lst)
    (if (null? lst)
      lst
      (cons (f (car lst)) (map f (cdr lst))))))

(define foldr
  (lambda (f base lst)
    (letrec ((foldr-aux
      (lambda (lst)
        (if (null? lst)
            base
            (f (car lst) (foldr-aux (cdr lst)))))))
    (foldr-aux lst))))

(define foldl
  (lambda (f base lst)
    (letrec ((foldl-aux
      (lambda (base lst)
        (if (null? lst)
          base
          (foldl-aux (f base (car lst)) (cdr lst))))))
    (foldl-aux base lst))))

(define for
  (lambda (lo hi f)
    (letrec ((for-aux
      (lambda (lo)
        (if (< lo hi)
            (cons (f lo) (for-aux (+ lo 1)))
            (list)))))
    (for-aux lo))))

(define concat
  (lambda (lists)
    (foldr (lambda (a b) (append a b)) (list) lists)))

(define list-read
  (lambda (lst i)
    (if (= i 0)
        (car lst)
        (list-read (cdr lst) (- i 1)))))

(define list-write
  (lambda (lst i val)
    (if (= i 0)
        (cons val (cdr lst))
        (cons (car lst) (list-write (cdr lst) (- i 1) val)))))

(define list-remove-pos
  (lambda (lst i)
    (if (= i 0)
        (cdr lst)
        (cons (car lst) (list-remove-pos (cdr lst) (- i 1))))))

(define member-of
  (lambda (e lst)
    (if (null? lst)
      #f
      (if (equal? e (car lst))
        #t
        (member-of e (cdr lst))))))

(define duplicates?
  (lambda (lst)
    (if (null? lst)
        #f
        (or (member-of (car lst) (cdr lst))
            (duplicates? (cdr lst))))))

(define make-matrix
  (lambda (n m init)
    (for 0 n (lambda (i) (for 0 m (lambda (j) (init i j)))))))

(define matrix-read
  (lambda (mat i j)
    (list-read (list-read mat i) j)))

(define matrix-write
  (lambda (mat i j val)
    (list-write mat i (list-write (list-read mat i) j val))))

(define matrix-size
  (lambda (mat)
    (cons (length mat) (length (car mat)))))

(define matrix-map
  (lambda (f mat)
    (map (lambda (lst) (map f lst)) mat)))

(define initial-random 0)

(define next-random
  (lambda (current-random)
    (modulo (+ (* current-random 3581) 12751) 131072)))

(define neighboring-cavities
  (lambda (pos cave)
    (let ((size (matrix-size cave)))
      (let ((n (car size)) (m (cdr size)))
        (let ((i (car pos)) (j (cdr pos)))
          (append (if (and (> i 0) (matrix-read cave (- i 1) j))
                      (list (cons (- i 1) j))
                      (list))
                  (if (and (< i (- n 1)) (matrix-read cave (+ i 1) j))
                      (list (cons (+ i 1) j))
                      (list))
                  (if (and (> j 0) (matrix-read cave i (- j 1)))
                      (list (cons i (- j 1)))
                      (list))
                  (if (and (< j (- m 1)) (matrix-read cave i (+ j 1)))
                      (list (cons i (+ j 1)))
                      (list))))))))

(define shuffle-aux
  (lambda (lst current-random)
    (if (null? lst)
        (list)
        (let ((new-random (next-random current-random)))
          (let ((i (modulo new-random (length lst))))
            (cons (list-read lst i)
                  (shuffle-aux (list-remove-pos lst i)
                               new-random)))))))

(define shuffle
  (lambda (lst)
    (shuffle-aux lst initial-random)))

(define cave-to-maze
  (lambda (cave)
    (matrix-map (lambda (x) (if x '_ '*)) cave)))

(define change-cavity-aux
  (lambda (cave pos new-cavity-id old-cavity-id)
    (let ((i (car pos)) (j (cdr pos)))
      (let ((cavity-id (matrix-read cave i j)))
        (if (equal? cavity-id old-cavity-id)
            (foldl (lambda (c nc)
                     (change-cavity-aux c nc new-cavity-id old-cavity-id))
                   (matrix-write cave i j new-cavity-id)
                   (neighboring-cavities pos cave))
            cave)))))

(define change-cavity
  (lambda (cave pos new-cavity-id)
    (let ((i (car pos)) (j (cdr pos)))
      (change-cavity-aux cave pos new-cavity-id (matrix-read cave i j)))))

(define pierce
  (lambda (pos cave)
    (let ((i (car pos)) (j (cdr pos)))
      (matrix-write cave i j pos))))

(define try-to-pierce
  (lambda (pos cave)
    (let ((i (car pos)) (j (cdr pos)))
      (let ((ncs (neighboring-cavities pos cave)))
        (if (duplicates?
             (map (lambda (nc) (matrix-read cave (car nc) (cdr nc))) ncs))
            cave
            (pierce pos
                    (foldl (lambda (c nc) (change-cavity c nc pos))
                           cave
                           ncs)))))))

(define pierce-randomly
  (lambda (possible-holes cave)
    (if (null? possible-holes)
        cave
        (let ((hole (car possible-holes)))
          (pierce-randomly (cdr possible-holes)
                           (try-to-pierce hole cave))))))

(define make-maze
  (lambda (n m)
    (if (not (and (odd? n) (odd? m)))
        (print 'error)
        (let ((cave
               (make-matrix n m (lambda (i j)
                                  (if (and (even? i) (even? j))
                                      (cons i j)
                                      #f))))
              (possible-holes
               (concat
                (for 0 n (lambda (i)
                           (concat
                            (for 0 m (lambda (j)
                                       (if (equal? (even? i) (even? j))
                                           (list)
                                           (list (cons i j)))))))))))
          (pierce-randomly (shuffle possible-holes) cave)))))
          
(define neighboring-cavities2
  (lambda (pos cave)
    (let ((size (matrix-size cave)))
      (let ((n (car size)) (m (cdr size)))
        (let ((i (car pos)) (j (cdr pos)))
          (append (if (and (> i 0) (matrix-read cave (- i 1) j))
                      (list (cons (- i 1) j))
                      (list))
                  (if (and (< i (- n 1)) (matrix-read cave (+ i 1) j))
                      (list (cons (+ i 1) j))
                      (list))
                  (if (and (> j 0) (matrix-read cave i (- j 1)))
                      (list (cons i (- j 1)))
                      (list))
                  (if (and (< j (- m 1)) (matrix-read cave i (+ j 1)))
                      (list (cons i (+ j 1)))
                      (list))))))))

(define shuffle-aux2
  (lambda (lst current-random)
    (if (null? lst)
        (list)
        (let ((new-random (next-random current-random)))
          (let ((i (modulo new-random (length lst))))
            (cons (list-read lst i)
                  (shuffle-aux (list-remove-pos lst i)
                               new-random)))))))

(define shuffle2
  (lambda (lst)
    (shuffle-aux2 lst initial-random)))

(define cave-to-maze2
  (lambda (cave)
    (matrix-map (lambda (x) (if x '_ '*)) cave)))

(define change-cavity-aux2
  (lambda (cave pos new-cavity-id old-cavity-id)
    (let ((i (car pos)) (j (cdr pos)))
      (let ((cavity-id (matrix-read cave i j)))
        (if (equal? cavity-id old-cavity-id)
            (foldl (lambda (c nc)
                     (change-cavity-aux c nc new-cavity-id old-cavity-id))
                   (matrix-write cave i j new-cavity-id)
                   (neighboring-cavities pos cave))
            cave)))))

(define change-cavity2
  (lambda (cave pos new-cavity-id)
    (let ((i (car pos)) (j (cdr pos)))
      (change-cavity-aux2 cave pos new-cavity-id (matrix-read cave i j)))))

(define pierce2
  (lambda (pos cave)
    (let ((i (car pos)) (j (cdr pos)))
      (matrix-write cave i j pos))))

(define try-to-pierce2
  (lambda (pos cave)
    (let ((i (car pos)) (j (cdr pos)))
      (let ((ncs (neighboring-cavities2 pos cave)))
        (if (duplicates?
             (map (lambda (nc) (matrix-read cave (car nc) (cdr nc))) ncs))
            cave
            (pierce pos
                    (foldl (lambda (c nc) (change-cavity2 c nc pos))
                           cave
                           ncs)))))))

(define pierce-randomly2
  (lambda (possible-holes cave)
    (if (null? possible-holes)
        cave
        (let ((hole (car possible-holes)))
          (pierce-randomly2 (cdr possible-holes)
                           (try-to-pierce2 hole cave))))))

(define make-maze2
  (lambda (n m)
    (if (not (and (odd? n) (odd? m)))
        (print 'error)
        (let ((cave
               (make-matrix n m (lambda (i j)
                                  (if (and (even? i) (even? j))
                                      (cons i j)
                                      #f))))
              (possible-holes
               (concat
                (for 0 n (lambda (i)
                           (concat
                            (for 0 m (lambda (j)
                                       (if (equal? (even? i) (even? j))
                                           (list)
                                           (list (cons i j)))))))))))
          (pierce-randomly2 (shuffle2 possible-holes) cave)))))

(begin (pretty-print (make-maze 5 5))
       (pretty-print (make-maze2 5 5)))



