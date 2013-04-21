;;; -*- Mode: Lisp; Syntax: Common-Lisp; Package: LANGBAND -*-

#|

DESC: lib/vanilla/levels.lisp - level customisation for Vanilla
Copyright (c) 2000-2001 - Stig Erik Sand�

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

|#

(in-package :langband)

(eval-when (:compile-toplevel :load-toplevel :execute)

  (defclass van-town-level (themed-level)
    ((id :initform 'town-level))
    ))

(defmethod level-ready? ((level van-town-level))
  (when (level.dungeon level)
    t))


(defmethod generate-level! ((level van-town-level) player dun)
  "Generates a town and returns it.  If the dungeon
argument is non-NIL it will be re-used and returned as
part of the new level."

  (let* ((*level* level)
	 (settings (get-setting :random-level)) ;; hack
	 (max-dungeon-width  (slot-value settings 'max-width))
	 (max-dungeon-height (slot-value settings 'max-height))
	 (dungeon (if dun dun (create-dungeon max-dungeon-width
					      max-dungeon-height
					      :its-depth 0)))
	 (qy +screen-height+)
	 (qx +screen-width+))

    (declare (type u-fixnum qy qx))

;;    (warn "Generating town with random state equal to ~s" *random-state*)
    
;;    (setf (player.map player) (make-map dungeon))
    
    (fill-dungeon-with-feature! dungeon +feature-perm-solid+)
    
    (fill-dungeon-part-with-feature! dungeon +feature-floor+
				     (cons (1+ +screen-width+)  (+ +screen-width+ qx -1))
				     (cons (1+ +screen-height+) (+ +screen-height+ qy -1)))

    ;; we need stores
    (let ((room-numbers (stable-sort (loop for x of-type u-fixnum from 0 to (1- +max-stores+)
					   collecting x)
				     #'(lambda (x y)
					 (declare (ignore x y))
					 (let ((val (random 100)))
					   (oddp val))))))
      (dotimes (y 2)
	(dotimes (x 4)
	  (store-build! dungeon (pop room-numbers) x y))))



    (loop named place-the-player
	  for x of-type u-fixnum = (with-type u-fixnum (+ qx (rand-range 3 (- +screen-width+ 4))))
	  for y of-type u-fixnum = (with-type u-fixnum (+ qy (rand-range 3 (- +screen-height+ 4))))
	  do
	  (when (cave-boldly-naked? dungeon x y)
	    (setf (cave-feature dungeon x y) +feature-more+)
	    (place-player! dungeon player x y)
	    (return-from place-the-player nil)))

    
    (setf (level.dungeon level) dungeon)

    level))


(defvar *van-saved-town-seed* nil)

(defun van-make-town-level-obj (old-level player)
  "A sucky function which should be simplified greatly."

  (flet ((do-generation (seed)
	   (let* ((town (make-instance 'van-town-level :depth 0))
		  (cl:*random-state* (cl:make-random-state seed)))

	     (generate-level! town player
			      (if old-level (level.dungeon old-level) nil)))))
  
    ;; we already have a saved value
    (cond (*van-saved-town-seed*
	   (do-generation *van-saved-town-seed*))
	 
	  ;; this is the first time
	  (t
	   (let* ((new-state (cl:make-random-state t)))
	     (setf *van-saved-town-seed* new-state)
	     (do-generation new-state)))
	  )))


(defmethod create-appropriate-level ((variant vanilla-variant) old-level player depth)

  (let ((level (if (= depth 0)
		   (van-make-town-level-obj old-level player)
		   (make-random-level-obj depth))))

    (unless (level-ready? level)
      (warn "Generating level ~a" level)
      (generate-level! level player
		       (if old-level (level.dungeon old-level) nil)))

    (assert (level-ready? level))
    
    level))


(defmethod print-depth ((level van-town-level) setting)
  "prints current depth somewhere"
  (declare (ignore setting))
  (c-prt "Town" *last-console-line* 70)) ;;fix 


(defmethod activate-object :after ((level van-town-level) &key)
  
  (let* ((dungeon (level.dungeon level))
	 (player *player*)
	 (var-obj *variant*)
	 (turn (variant.turn var-obj))
	 (mod-time (mod turn (variant.day-length var-obj)))
	 ;; quick hack
	 (time-of-day (if (< mod-time (variant.twilight var-obj))
			  'day
			  'night)))
	 

    ;; hackish
    (let ((resident-num (if (eq time-of-day 'day) 4 8)))
	
      ;; add some inhabitants
      (dotimes (i resident-num)
	(allocate-monster! dungeon player 3 t)))

    (van-town-illuminate! dungeon player time-of-day)
    ;;(warn "post activate")
    ))


(defun van-town-illuminate! (dungeon player time-of-day)
  "Illuminates the town according to the time-of-day."
  (declare (ignore player))
  
    (with-dungeon (dungeon (coord x y))
      (declare (ignore x y))
      (let ((feat (coord.feature coord)))
	;; slightly interesting grids
	(cond ((> feat +feature-invisible+)
	       (bit-flag-add! (coord.flags coord) #.(logior +cave-glow+ +cave-mark+)))
	      ;; day-time
	      ((eq time-of-day 'day)
	       (bit-flag-add! (coord.flags coord) +cave-glow+))
	      ;; at night
	      (t
	       (bit-flag-remove! (coord.flags coord) +cave-glow+))
	      ))
      ;; skip doorways yet
      )

    (bit-flag-add! *update* +forget-view+ +update-view+)
    (bit-flag-add! *redraw* +print-map+)


  )
	 

(defmethod get-otype-table ((level random-level) var-obj)
  (%get-var-table var-obj 'level 'objects))

(defmethod get-otype-table ((level van-town-level) var-obj)
  (%get-var-table var-obj 'level 'objects))

(defmethod get-mtype-table ((level random-level) var-obj)
  (%get-var-table var-obj level 'monsters))

(defmethod get-mtype-table ((level van-town-level) var-obj)
  (%get-var-table var-obj level 'monsters))

(defmethod get-mtype-table ((level (eql 'random-level)) var-obj)
  (%get-var-table var-obj level 'monsters))

(defmethod get-mtype-table ((level (eql 'town-level)) var-obj)
  (%get-var-table var-obj level 'monsters))
