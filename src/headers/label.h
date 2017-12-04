/*
 * This file is part of the Charly Virtual Machine (https://github.com/KCreate/charly-vm)
 *
 * MIT License
 *
 * Copyright (c) 2017 Leonard Schütz
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "block.h"

#pragma once

namespace Charly {

/*
 * Stores an offset and length into the generated instruction stream of a block
 * Can be used to compute offsets for the several branching instructions
 * */
template <class T>
class BlockLabel {
private:
  InstructionBlock& block;

public:
  uint32_t offset;

  BlockLabel(InstructionBlock& t_block) : block(t_block), offset(t_block.writeoffset) {
  }
  BlockLabel(InstructionBlock& t_block, uint32_t offset) : block(t_block), offset(offset) {
  }
  BlockLabel(const BlockLabel& other) : block(other.block), offset(other.offset) {
  }

  inline BlockLabel& write(T& arg) {
    *static_cast<T*>(this->block.data + this->offset) = arg;
    return *this;
  }

  inline BlockLabel& write_current_block_offset() {
    *reinterpret_cast<T*>(this->block.data + this->offset) = static_cast<T>(this->block.writeoffset - this->offset);
    return *this;
  }
};

}
