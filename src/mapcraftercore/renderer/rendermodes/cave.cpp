/*
 * Copyright 2012-2015 Moritz Hilscher
 *
 * This file is part of Mapcrafter.
 *
 * Mapcrafter is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mapcrafter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Mapcrafter.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "cave.h"

namespace mapcrafter {
namespace renderer {

CaveRendermode::CaveRendermode(const RenderState& state, bool high_contrast)
	: Rendermode(state), high_contrast(high_contrast) {
}

CaveRendermode::~CaveRendermode() {
}

bool CaveRendermode::isLight(const mc::BlockPos& pos) {
	return state.getBlock(pos, mc::GET_SKY_LIGHT).sky_light > 0;
}

bool CaveRendermode::isTransparentBlock(const mc::Block& block) const {
	return block.id == 0 || state.images->isBlockTransparent(block.id, block.data);
}

bool CaveRendermode::isHidden(const mc::BlockPos& pos, uint16_t id, uint16_t data) {
	mc::BlockPos directions[6] = {
			mc::DIR_NORTH, mc::DIR_SOUTH, mc::DIR_EAST, mc::DIR_WEST,
			mc::DIR_TOP, mc::DIR_BOTTOM
	};
	// check if this block touches sky light
	for (int i = 0; i < 6; i++) {
		if (isLight(pos + directions[i]))
			return true;
	}

	// water and blocks under water are a special case
	// because water is transparent, the renderer thinks this is a visible part of a cave
	// we need to check if there is sunlight on the surface of the water
	// if yes => no cave, hide block
	// if no  => lake in a cave, show it
	mc::Block top = state.getBlock(pos + mc::DIR_TOP,
			mc::GET_ID | mc::GET_DATA | mc::GET_SKY_LIGHT);
	if (id == 8 || id == 9 || top.id == 8 || top.id == 9) {
		mc::BlockPos p = pos + mc::DIR_TOP;
		mc::Block block(top.id, top.data, 0, 0, top.sky_light);

		while (block.id == 8 || block.id == 9) {
			block = state.getBlock(p, mc::GET_ID | mc::GET_DATA | mc::GET_SKY_LIGHT);
			p.y++;
		}

		if (block.sky_light > 0)
			return true;
	}

	mc::Block south, west;
	south = state.getBlock(pos + mc::DIR_SOUTH);
	west = state.getBlock(pos + mc::DIR_WEST);

	// show all blocks, which don't touch sunlight
	// and have a transparent block on the south, west or top side
	// south, west and top, because with this you can look in the caves
	if (isTransparentBlock(south)
			|| isTransparentBlock(west)
			|| isTransparentBlock(top)) {
		return false;
	}

	return true;
}

void CaveRendermode::draw(RGBAImage& image, const mc::BlockPos& pos,
		uint16_t id, uint16_t data) {
	// a nice color gradient to see something
	// (because the whole map is just full of cave stuff,
	// one can't differentiate the single caves)

	double h1 = (double) (64 - pos.y) / 64;
	if (pos.y > 64)
		h1 = 0;

	double h2 = 0;
	if (pos.y >= 64 && pos.y < 96)
		h2 = (double) (96 - pos.y) / 32;
	else if (pos.y > 16 && pos.y < 64)
		h2 = (double) (pos.y - 16) / 48;

	double h3 = 0;
	if (pos.y > 64)
		h3 = (double) (pos.y - 64) / 64;

	int r = h1 * 128.0 + 128.0;
	int g = h2 * 255.0;
	int b = h3 * 255.0;

	if (high_contrast) {
		// if high contrast mode is enabled, then do some magic here

		// get luminance of recolor
		int luminance = (10*r + 3*g + b) / 14;

		// try to do luminance-neutral additive/subtractive color
		// instead of alpha blending (for better contrast)
		// so first subtract luminance from each component
		r = (r - luminance) / 3; // /3 is similar to alpha=85
		g = (g - luminance) / 3;
		b = (b - luminance) / 3;

		int size = image.getWidth();
		for (int y = 0; y < size; y++) {
			for (int x = 0; x < size; x++) {
				uint32_t pixel = image.getPixel(x, y);
				if (pixel != 0)
					image.setPixel(x, y, rgba_add_clamp(pixel, r, g, b));
			}
		}
	} else {
		// otherwise just simple alphablending
		uint32_t color = rgba(r, g, b, 128);

		int size = image.getWidth();
		for (int y = 0; y < size; y++) {
			for (int x = 0; x < size; x++) {
				uint32_t pixel = image.getPixel(x, y);
				if (pixel != 0)
					image.blendPixel(color, x, y);
			}
		}
	}
}

} /* namespace render */
} /* namespace mapcrafter */
