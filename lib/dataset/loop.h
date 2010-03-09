/*
    Copyright 2009 Brain Research Institute, Melbourne, Australia

    Written by J-Donald Tournier, 16/10/09.

    This file is part of MRtrix.

    MRtrix is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MRtrix is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MRtrix.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef __dataset_loop_h__
#define __dataset_loop_h__

#include "progressbar.h"
#include "dataset/position.h"

namespace MR {
  namespace DataSet {

    /** \addtogroup DataSet
      @{ */

    /** \defgroup loop Looping functions
      @{ */

    //! a class to loop over arbitrary numbers of axes of a DataSet
    /*! This class can be used to loop over any number of axes of one of more
     * DataSets. Its use is best illustrated with the following examples.
     *
     * If \a vox in the following example is a 3D DataSet (i.e. vox.ndim() ==
     * 3), then:
     * \code
     * float sum = 0.0;
     * Loop loop;
     * for (loop.start (vox); loop.ok(); loop.next (vox))
     *   sum += vox.value();
     * \endcode
     * is equivalent to:
     * \code
     * float sum = 0.0;
     * for (vox[2] = 0; vox[2] < vox.dim(2); ++vox[2])
     *   for (vox[1] = 0; vox[1] < vox.dim(1); ++vox[1])
     *     for (vox[0] = 0; vox[0] < vox.dim(0); ++vox[0])
     *       sum += vox.value();
     * \endcode
     * This has the advantage that the dimensionality of the DataSet does not
     * need to be known at compile-time. In other words, if the DataSet was
     * 4-dimensional, the first looping construct would correctly iterate over
     * all voxels, whereas the second one would only process the first 3D
     * volume.
     *
     * \section multiloop Looping over multiple DataSets:
     * It is often required to loop over more than one DataSet of the same
     * dimension. This is done trivially by passing any additional DataSets to
     * be looped over to both the start() and next() member functions. For
     * example, this code snippet will copy the contents of the DataSet \a src
     * into a DataSet \a dest, assumed to have the same dimensions as \a src:
     * \code
     * Loop loop;
     * for (loop.start (dest, src); loop.ok(); loop.next (dest, src))
     *   dest.value() = vox.value();
     * \endcode
     *
     * \section restrictedloop Looping over a specific range of axes
     * It is also possible to explicitly specify the range of axes to be looped
     * over. In the following example, the program will loop over each 3D
     * volume in the DataSet in turn:
     * \code
     * Loop outer (3); // outer loop iterates over axes 3 and above
     * for (outer.start (vox); outer.ok(); outer.next (vox)) {
     *   float sum = 0.0;
     *   Loop inner (0, 3); // inner loop iterates over axes 0 to 3
     *   for (inner.start (vox); inner.ok(); inner.next (vox))
     *     sum += vox.value();
     *   print ("total = " + str (sum) + "\n");
     * }
     * \endcode
     * 
     * \section progressloop Displaying progress status
     * The Loop object can also display its progress as it proceeds, using the
     * appropriate constructor. In the following example, the program will
     * display its progress as it averages a DataSet:
     * \code
     * float sum = 0.0;
     *
     * Loop loop ("averaging..."); 
     * for (loop.start (vox); loop.ok(); loop.next (vox)) 
     *   sum += vox.value();
     *
     * float average = sum / float (DataSet::voxel_count (vox));
     * print ("average = " + str (average) + "\n");
     * \endcode
     * The output would look something like this:
     * \code
     * myprogram: averaging... 100%
     * average = 23.42
     * \endcode
     * 
     * \sa LoopInOrder
     */
    class Loop 
    {
      public:
        //! Constructor
        /*! Construct a Loop object to iterate over the axes specified. By
         * default, the Loop will iterate over all axes of the first DataSet
         * supplied to next(). */
        Loop (size_t from_axis = 0, size_t to_axis = SIZE_MAX) : 
          from_ (from_axis), to_ (to_axis), cont_ (true), progress_ (false) { }

        //! Constructor with progress status
        /*! Construct a Loop object to iterate over the axes specified and
         * display the progress status with the specified message. By default,
         * the Loop will iterate over all axes of the first DataSet supplied to
         * next(). */
        Loop (const std::string& message, size_t from_axis = 0, size_t to_axis = SIZE_MAX) :
          from_ (from_axis), to_ (to_axis), cont_ (true), progress_ (true), progress_message_ (message) { }

        //! Start the loop to iterate over a single DataSet
        /*! Start the loop by resetting the appropriate coordinates of each of
         * the specified DataSets to zero, and initialising the progress status
         * if appropriate. Note that only those axes specified in the Loop
         * constructor will have their coordinates set to zero; the coordinates
         * of all other axes will be left untouched. */
        template <class Set> 
          inline void start (Set& set)
          {
            cont_ = true;
            for (size_t n = from_; n < std::min (set.ndim(), to_); ++n)
              set[n] = 0;
            if (progress_)
              ProgressBar::init (voxel_count (set, from_, to_), progress_message_);
          }
        //! Start the loop to iterate over two DataSets
        /*! \copydetails Loop::start(Set&) */
        template <class Set, class Set2> 
          inline void start (Set& set, Set2& set2)
          {
            cont_ = true;
            for (size_t n = from_; n < std::min (set.ndim(), to_); ++n) {
              set[n] = 0;
              set2[n] = 0;
            }
            if (progress_)
              ProgressBar::init (voxel_count (set, from_, to_), progress_message_);
          }

        //! Start the loop to iterate over three DataSets
        /*! \copydetails Loop::start(Set&) */
        template <class Set, class Set2, class Set3> 
          inline void start (Set& set, Set2& set2, Set3& set3) 
          {
            cont_ = true;
            for (size_t n = from_; n < std::min (set.ndim(), to_); ++n) {
              set[n] = 0;
              set2[n] = 0;
              set3[n] = 0;
            }
            if (progress_)
              ProgressBar::init (voxel_count (set, from_, to_), progress_message_);
          }

        //! Check whether the loop should continue iterating
        /*! \return true if the loop has not completed, false otherwise. */
        bool ok () const { return (cont_); }

        //! Proceed to next iteration for a single DataSet
        /*! Advance coordinates of all specified DataSets to the next position
         * to be processed, and update the progress status if appropriate. */
        template <class Set> 
          void next (Set& set)
          { 
            next_impl (from_, set);
            if (cont_ && progress_) ProgressBar::inc();
          }

        //! Proceed to next iteration for two DataSets
        /*! \copydetails Loop::next(Set&) */
        template <class Set, class Set2> 
          void next (Set& set, Set2& set2)
          {
            next_impl (from_, set, set2);
            if (cont_ && progress_) ProgressBar::inc();
          }

        //! Proceed to next iteration for three DataSets
        /*! \copydetails Loop::next(Set&) */
        template <class Set, class Set2, class Set3> 
          void next (Set& set, Set2& set2, Set3& set3)
          {
            next_impl (from_, set, set2, set3);
            if (cont_ && progress_) ProgressBar::inc();
          }

      private:
        const size_t from_, to_;
        bool cont_;
        const bool progress_;
        const std::string progress_message_;

        template <class Set> 
          void next_impl (size_t axis, Set& set)
          { 
            if (set[axis] + 1 < set.dim(axis)) ++set[axis]; 
            else {
              if (axis+1 == std::min (to_,set.ndim())) {
                cont_ = false;
                if (progress_) ProgressBar::done();
              }
              else {
                next_impl (axis+1, set);
                if (cont_) set[axis] = 0;
              }
            }
          }

        template <class Set, class Set2> 
          void next_impl (size_t axis, Set& set, Set2& set2) 
          { 
            if (set[axis] + 1 < set.dim(axis)) { ++set[axis]; ++set2[axis]; }
            else {
              if (axis+1 == std::min (to_, set.ndim())) {
                cont_ = false;
                if (progress_) ProgressBar::done();
              }
              else {
                next_impl (axis+1, set, set2);
                if (cont_) { set[axis] = 0; set2[axis] = 0; }
              }
            }
          }

        template <class Set, class Set2, class Set3> 
          void next_impl (size_t axis, Set& set, Set2& set2, Set3& set3)
          { 
            if (set[axis] + 1 < set.dim(axis)) { ++set[axis]; ++set2[axis]; ++set3[axis]; }
            else {
              if (axis+1 == std::min (to_, set.ndim())) {
                cont_ = false; 
                if (progress_) ProgressBar::done();
              }
              else {
                next_impl (axis+1, set, set2, set3);
                if (cont_) { set[axis] = 0; set2[axis] = 0; set3[axis] = 0; }
              }
            }
          }

    };


    //! @}
    //! @}
  }
}

#endif


