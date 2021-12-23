#pragma once

#include <fmt/format.h>

#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

namespace fmt {
    template <int N, typename T>
    struct formatter<glm::vec<N, T>> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(glm::vec<N, T> const& v, FormatContext &ctx) {
            format_to(ctx.out(), "{{");
            for (int i = 0; i < N; i++) {
                format_to(ctx.out(), "{}{}", v[i], ((i < (N - 1)) ? "," : ""));
            }
            return format_to(ctx.out(), "}}");
        }
    };

    template <int N, typename T>
    struct formatter<glm::mat<N, N, T>> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(glm::mat<N, N, T> const& m, FormatContext &ctx) {
            format_to(ctx.out(), "{{");
            for (int i = 0; i < N; i++) {
                format_to(ctx.out(), "{}{}", m[i], ((i < N - 1) ? "," : ""));
            }
            return format_to(ctx.out(), "}}");
        }
    };
}

#include "aoc.h"
