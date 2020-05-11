/**
 * This file is part of the "libterminal" project
 *   Copyright (c) 2020 Christian Parpart <christian@parpart.family>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <crispy/text/UnicodeTraits.h>
#include <crispy/text/UnicodeTables.h>

#include <string_view>

namespace crispy::text {

/// Implements http://www.unicode.org/reports/tr29/tr29-27.html#Grapheme_Cluster_Boundary_Rules
class GraphemeSegmenter {
  public:
    constexpr GraphemeSegmenter(char32_t const* _begin, char32_t const* _end) noexcept
      : left_{ _begin },
        right_{ _begin },
        end_{ _end }
    {
        ++*this;
    }

    constexpr GraphemeSegmenter(std::u32string_view const& _sv) noexcept
      : GraphemeSegmenter(_sv.begin(), _sv.end())
    {}

    constexpr GraphemeSegmenter() noexcept
      : GraphemeSegmenter({}, {})
    {}

    constexpr GraphemeSegmenter& operator++()
    {
        right_ = left_;
        // TODO
        return *this;
    }

    constexpr std::u32string_view operator*() const noexcept
    {
        return std::u32string_view(left_, right_ - left_);
    }

    constexpr bool empty() const noexcept
    {
        return right_ == end_;
    }

    constexpr bool operator==(GraphemeSegmenter const& _rhs) const noexcept
    {
        return (empty() && _rhs.empty())
            || (left_ == _rhs.left_ && right_ == _rhs.right_);
    }

    /// Tests if codepoint @p a and @p b are breakable, and thus, two different grapheme clusters.
    ///
    /// @retval true both codepoints to not belong to the same grapheme cluster
    /// @retval false both codepoints belong to the same grapheme cluster
    static constexpr bool breakable(char32_t a, char32_t b)
    {
        constexpr char32_t CR = 0x000D;
        constexpr char32_t LF = 0x000A;
        constexpr char32_t ZWJ = 0x200D;

        // GB3: Do not break between a CR and LF. Otherwise, break before and after controls.
        if (a == CR && b == LF) // GB3
            return false;

        // GB4
        if (a == CR || a == LF || control(a))
            return false;

        // GB5
        if (b == CR || b == LF || control(b))
            return false;

        // Do not break Hangul syllable sequences.
        // GB6: TODO
        // GB7: TODO
        // GB8: TODO

        // GB9: Do not break before extending characters.
        if (extend(b) || b == ZWJ) // GB9
            return false;

        // GB9a: Do not break before SpacingMarks
        if (spacingMark(b))
            return false;

        // GB9b: or after Prepend characters.
        // (NB: wrt "Prepend": Currently there are no characters with this value)
        if (false/*contains(General_Category::Pepend, a)*/) // GB9b
            return false;

        // GB11: Do not break within emoji modifier sequences or emoji zwj sequences.
        if (a == ZWJ && extended_pictographic(b))
            return false;

        // GB12/GB13: Do not break within emoji flag sequences.
        // That is, do not break between regional indicator (RI) symbols
        // if there is an odd number of RI characters before the break point.
        if (isRegionalIndicator(a) && isRegionalIndicator(b)) // GB8a
            return false;

        // GB999: Otherwise, break everywhere.
        return true; // GB10
    }

    static constexpr bool nonbreakable(char32_t a, char32_t b)
    {
        return !breakable(a, b);
    }

  private:
    static bool extend(char32_t _codepoint) noexcept
    {
        return contains(Core_Property::Grapheme_Extend, _codepoint)
            || contains(General_Category::Spacing_Mark, _codepoint)
            || (emoji_modifier(_codepoint) && _codepoint != 0x200D);
    }

    static bool control(char32_t ch)
    {
        return contains(General_Category::Line_Separator, ch)
            || contains(General_Category::Paragraph_Separator, ch)
            || contains(General_Category::Control, ch)
            || contains(General_Category::Surrogate, ch)
            || (contains(General_Category::Unassigned, ch)
                    && contains(Core_Property::Default_Ignorable_Code_Point, ch))
            || (contains(General_Category::Format, ch)
                    && ch != 0x000D
                    && ch != 0x000A
                    && ch != 0x200C
                    && ch != 0x200D);
    }

    static bool spacingMark(char32_t _codepoint)
    {
        if (auto const p = grapheme_cluster_break(_codepoint);
                (!p.has_value() || *p != Grapheme_Cluster_Break::Extend)
                && (contains(General_Category::Spacing_Mark, _codepoint)
                    || _codepoint == 0x0E33
                    || _codepoint == 0x0EB3))
            return true;
        return false;
    }

  private:
    char32_t const* left_;
    char32_t const* right_;
    char32_t const* end_;
};

} // end namespace