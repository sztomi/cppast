// Copyright (C) 2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_class.hpp>

#include "test_parser.hpp"

using namespace cppast;

TEST_CASE("cpp_class")
{
    auto code = R"(
// basic
struct a {};
class b final {};
union c {};

struct ignore_me;

// members
struct d
{
    enum m1 {};

public:
    enum m2 {};

private:
private:
    enum m3 {};

protected:
    enum m4 {};
};

// bases
class e
: a, private d {};

namespace ns
{
    struct base {};
}

struct f
: public ns::base, virtual protected e
{};

using namespace ns;

struct g
: base {};
)";

    cpp_entity_index idx;
    auto             file = parse(idx, "cpp_class.cpp", code);
    auto count            = test_visit<cpp_class>(*file, [&](const cpp_class& c) {
        if (c.name() == "a" || c.name() == "base")
        {
            REQUIRE(c.class_kind() == cpp_class_kind::struct_t);
            REQUIRE(!c.is_final());
            REQUIRE(c.bases().begin() == c.bases().end());
            REQUIRE(c.begin() == c.end());
        }
        else if (c.name() == "b")
        {
            REQUIRE(c.class_kind() == cpp_class_kind::class_t);
            REQUIRE(c.is_final());
            REQUIRE(c.bases().begin() == c.bases().end());
            REQUIRE(c.begin() == c.end());
        }
        else if (c.name() == "c")
        {
            REQUIRE(c.class_kind() == cpp_class_kind::union_t);
            REQUIRE(!c.is_final());
            REQUIRE(c.bases().begin() == c.bases().end());
            REQUIRE(c.begin() == c.end());
        }
        else if (c.name() == "d")
        {
            REQUIRE(c.class_kind() == cpp_class_kind::struct_t);
            REQUIRE(!c.is_final());
            REQUIRE(c.bases().begin() == c.bases().end());

            auto no_children = 0u;
            for (auto& child : c)
            {
                switch (no_children++)
                {
                case 0:
                    REQUIRE(child.name() == "m1");
                    break;
                case 1:
                    REQUIRE(child.name() == "public");
                    REQUIRE(child.kind() == cpp_entity_kind::access_specifier_t);
                    REQUIRE(static_cast<const cpp_access_specifier&>(child).access_specifier()
                            == cpp_public);
                    break;
                case 2:
                    REQUIRE(child.name() == "m2");
                    break;
                case 3:
                    REQUIRE(child.name() == "private");
                    REQUIRE(child.kind() == cpp_entity_kind::access_specifier_t);
                    REQUIRE(static_cast<const cpp_access_specifier&>(child).access_specifier()
                            == cpp_private);
                    break;
                case 4:
                    REQUIRE(child.name() == "private");
                    REQUIRE(child.kind() == cpp_entity_kind::access_specifier_t);
                    REQUIRE(static_cast<const cpp_access_specifier&>(child).access_specifier()
                            == cpp_private);
                    break;
                case 5:
                    REQUIRE(child.name() == "m3");
                    break;
                case 6:
                    REQUIRE(child.name() == "protected");
                    REQUIRE(child.kind() == cpp_entity_kind::access_specifier_t);
                    REQUIRE(static_cast<const cpp_access_specifier&>(child).access_specifier()
                            == cpp_protected);
                    break;
                case 7:
                    REQUIRE(child.name() == "m4");
                    break;
                default:
                    REQUIRE(false);
                    break;
                }
            }
            REQUIRE(no_children == 8u);
        }
        else if (c.name() == "e")
        {
            REQUIRE(c.class_kind() == cpp_class_kind::class_t);
            REQUIRE(!c.is_final());
            REQUIRE(c.begin() == c.end());

            auto no_bases = 0u;
            for (auto& base : c.bases())
            {
                ++no_bases;
                if (base.name() == "a")
                {
                    REQUIRE(base.access_specifier() == cpp_private);
                    REQUIRE(!base.is_virtual());

                    auto entity = base.entity().get(idx);
                    REQUIRE(entity);
                    REQUIRE(entity.value().name() == "a");
                }
                else if (base.name() == "d")
                {
                    REQUIRE(base.access_specifier() == cpp_private);
                    REQUIRE(!base.is_virtual());

                    auto entity = base.entity().get(idx);
                    REQUIRE(entity);
                    REQUIRE(entity.value().name() == "d");
                }
                else
                    REQUIRE(false);
            }
            REQUIRE(no_bases == 2u);
        }
        else if (c.name() == "f")
        {
            REQUIRE(c.class_kind() == cpp_class_kind::struct_t);
            REQUIRE(!c.is_final());
            REQUIRE(c.begin() == c.end());

            auto no_bases = 0u;
            for (auto& base : c.bases())
            {
                ++no_bases;
                if (base.name() == "ns::base")
                {
                    REQUIRE(base.access_specifier() == cpp_public);
                    REQUIRE(!base.is_virtual());

                    auto entity = base.entity().get(idx);
                    REQUIRE(entity);
                    REQUIRE(entity.value().name() == "base");
                    REQUIRE(full_name(entity.value()) == "ns::base");
                }
                else if (base.name() == "e")
                {
                    REQUIRE(base.access_specifier() == cpp_protected);
                    REQUIRE(base.is_virtual());

                    auto entity = base.entity().get(idx);
                    REQUIRE(entity);
                    REQUIRE(entity.value().name() == "e");
                }
                else
                    REQUIRE(false);
            }
            REQUIRE(no_bases == 2u);
        }
        else if (c.name() == "g")
        {
            REQUIRE(c.class_kind() == cpp_class_kind::struct_t);
            REQUIRE(!c.is_final());
            REQUIRE(c.begin() == c.end());

            auto no_bases = 0u;
            for (auto& base : c.bases())
            {
                ++no_bases;
                if (base.name() == "base")
                {
                    REQUIRE(base.access_specifier() == cpp_public);
                    REQUIRE(!base.is_virtual());

                    auto entity = base.entity().get(idx);
                    REQUIRE(entity);
                    REQUIRE(entity.value().name() == "base");
                    REQUIRE(full_name(entity.value()) == "ns::base");
                }
                else
                    REQUIRE(false);
            }
            REQUIRE(no_bases == 1u);
        }
        else
            REQUIRE(false);
    });
    REQUIRE(count == 8u);
}