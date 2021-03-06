// Copyright (C) 2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef CPPAST_PREPROCESSOR_HPP_INCLUDED
#define CPPAST_PREPROCESSOR_HPP_INCLUDED

#include <cppast/cpp_preprocessor.hpp>
#include <cppast/libclang_parser.hpp>

namespace cppast
{
    namespace detail
    {
        struct pp_entity
        {
            std::unique_ptr<cpp_entity> entity;
            unsigned                    line;
        };

        struct pp_doc_comment
        {
            std::string comment;
            unsigned    line;
            enum
            {
                c,
                cpp,
                end_of_line,
            } kind;

            bool matches(const cpp_entity& e, unsigned line);
        };

        struct preprocessor_output
        {
            std::string                 source;
            std::vector<pp_entity>      entities;
            std::vector<pp_doc_comment> comments;
        };

        preprocessor_output preprocess(const libclang_compile_config& config, const char* path,
                                       const diagnostic_logger& logger);
    }
} // namespace cppast::detail

#endif // CPPAST_PREPROCESSOR_HPP_INCLUDED
