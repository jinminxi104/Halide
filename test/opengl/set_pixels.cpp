#include <Halide.h>
#include <stdio.h>
#include <stdlib.h>

using namespace Halide;

int main() {

    // This test must be run with HL_JIT_TARGET=host-opengl
    const char* jit_target_env = getenv("HL_JIT_TARGET");
    if (!jit_target_env || strcmp("host-opengl", jit_target_env))  {
        fprintf(stderr,"ERROR: This test must be run with HL_JIT_TARGET=host-opengl.\n");
        return 1;
    }

    Func f;
    Var x, y, c;

    f(x, y, c) = cast<uint8_t>(select(c == 0, 10*x + y,
                                      c == 1, 127, 12));

    Image<uint8_t> out(10, 10, 3);
    f.bound(c, 0, 3);
    f.glsl(x, y, c);
    f.unroll(c);
    f.realize(out);

    out.copy_to_host();
    for (int y=0; y<out.height(); y++) {
        for (int x=0; x<out.width(); x++) {
            if (!(out(x, y, 0) == 10*x+y && out(x, y, 1) == 127 && out(x, y, 2) == 12)) {
                fprintf(stderr, "Incorrect pixel (%d, %d, %d) at x=%d y=%d.\n",
                        out(x, y, 0), out(x, y, 1), out(x, y, 2),
                        x, y);
                return 1;
            }
        }
    }

    printf("Success!\n");
    return 0;
}
