//
// Created by lily on 7/17/21.
//

#ifndef SSXTOOLS_SHAPE_H
#define SSXTOOLS_SHAPE_H

#include <ssxtools/shps/Structures.h>
#include <ssxtools/shps/ChunkUnion.h>
#include <vector>

namespace ssxtools::shps {

	/**
	 * A Shape. This form is mostly for file introspection and conversion to a real image
	 * also TODO
	 */
	struct Shape {

	   private:
		/**
		 * All the chunks this shape is made up of.
		 */
		std::vector<ChunkUnion> chunks;

		/**
		 * The name of the shape.
		 * This is set to the TOC name-code by default, but if the shape has a full-name
		 * it will be set to that.
		 */
		std::string Name;
	};

}

#endif //SSXTOOLS_SHAPE_H
