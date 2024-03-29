/*=============================================================================
    Copyright (c) 2002 2004  2006Joel de Guzman
    Copyright (c) 2004 Eric Niebler
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "utils.hpp"
#include "actions_class.hpp"
#include "grammar_impl.hpp"
#include "block_tags.hpp"
#include "template_tags.hpp"
#include <boost/spirit/include/classic_assign_actor.hpp>
#include <boost/spirit/include/classic_if.hpp>
#include <boost/spirit/include/classic_clear_actor.hpp>
#include <boost/spirit/include/phoenix1_primitives.hpp>
#include <boost/spirit/include/phoenix1_casts.hpp>

namespace quickbook
{
    namespace cl = boost::spirit::classic;
    namespace ph = phoenix;

    struct block_element_grammar_local
    {
        cl::rule<scanner>
                        heading, inner_block, inner_phrase, def_macro,
                        table, table_title, table_row, variablelist,
                        varlistentry, varlistterm, list, cell,
                        preformatted, begin_section, end_section,
                        xinclude, include, include_filename,
                        template_, template_id, template_formal_arg,
                        template_body, identifier, import,
                        element_id, element_id_1_5, element_id_1_6,
                        same_line;
    };

    void quickbook_grammar::impl::init_block_elements()
    {
        block_element_grammar_local& local = cleanup_.add(
            new block_element_grammar_local);

        local.element_id =
            !(  ':'
            >>  (   !(qbk_since(105u) >> space)
                >>  (+(cl::alnum_p | '_'))      [actions.values.entry(ph::arg1, ph::arg2, general_tags::element_id)]
                |   cl::eps_p                   [actions.element_id_warning]
                )
            )
            ;
        
        local.element_id_1_5 =
                !(qbk_since(105u) >> local.element_id);

        local.element_id_1_6 =
                !(qbk_since(106u) >> local.element_id);

        elements.add
            ("section", element_info(element_info::block, &local.begin_section, block_tags::begin_section))
            ("endsect", element_info(element_info::block, &local.end_section, block_tags::end_section))
            ;

        local.begin_section =
                space
            >>  local.element_id
            >>  space
            >>  local.inner_phrase
            ;

        local.end_section =
                cl::eps_p
            ;

        local.heading
            =   space
            >>  local.element_id_1_6
            >>  space
            >>  local.inner_phrase
            ;

        elements.add
            ("heading", element_info(element_info::conditional_or_block, &local.heading, block_tags::generic_heading))
            ("h1", element_info(element_info::conditional_or_block, &local.heading, block_tags::heading1))
            ("h2", element_info(element_info::conditional_or_block, &local.heading, block_tags::heading2))
            ("h3", element_info(element_info::conditional_or_block, &local.heading, block_tags::heading3))
            ("h4", element_info(element_info::conditional_or_block, &local.heading, block_tags::heading4))
            ("h5", element_info(element_info::conditional_or_block, &local.heading, block_tags::heading5))
            ("h6", element_info(element_info::conditional_or_block, &local.heading, block_tags::heading6))
            ;

        elements.add
            ("blurb", element_info(element_info::nested_block, &local.inner_block, block_tags::blurb))
            (":", element_info(element_info::nested_block, &local.inner_block, block_tags::blockquote))
            ("warning", element_info(element_info::nested_block, &local.inner_block, block_tags::warning))
            ("caution", element_info(element_info::nested_block, &local.inner_block, block_tags::caution))
            ("important", element_info(element_info::nested_block, &local.inner_block, block_tags::important))
            ("note", element_info(element_info::nested_block, &local.inner_block, block_tags::note))
            ("tip", element_info(element_info::nested_block, &local.inner_block, block_tags::tip))
            ;

        elements.add
            ("block", element_info(element_info::nested_block, &local.inner_phrase, block_tags::block, 106u))
            ;

        elements.add
            ("pre", element_info(element_info::nested_block, &local.preformatted, block_tags::preformatted))
            ;

        local.preformatted =
                ( qbk_before(106) >> space
                | qbk_since(106) >> blank >> !eol
                )
            >>  actions.to_value()
                [
                    inside_preformatted
                ]
            ;

        elements.add
            ("def", element_info(element_info::conditional_or_block, &local.def_macro, block_tags::macro_definition))
            ;

        local.def_macro =
               space
            >> macro_identifier                 [actions.values.entry(ph::arg1, ph::arg2)]
            >> blank
            >> local.inner_phrase
            ;

        local.identifier =
            (cl::alpha_p | '_') >> *(cl::alnum_p | '_')
            ;

        local.template_id =
            local.identifier | (cl::punct_p - (cl::ch_p('[') | ']'))
            ;

        elements.add
            ("template", element_info(element_info::conditional_or_block, &local.template_, block_tags::template_definition))
            ;

        local.template_ =
               space
            >> local.template_id                [actions.values.entry(ph::arg1, ph::arg2)]
            >> actions.values.list()[
            !(
                space >> '['
                >> *(
                        space
                    >>  local.template_id       [actions.values.entry(ph::arg1, ph::arg2)]
                    )
                >> space >> ']'
            )
            ]
            >>  (   cl::eps_p(*cl::blank_p >> cl::eol_p)
                >>  local.template_body         [actions.values.entry(ph::arg1, ph::arg2, template_tags::block)]
                |   local.template_body         [actions.values.entry(ph::arg1, ph::arg2, template_tags::phrase)]
                )
            ;

        local.template_body =
           *(('[' >> local.template_body >> ']') | (cl::anychar_p - ']'))
            >> cl::eps_p(space >> ']')
            >> space
            ;

        elements.add
            ("variablelist", element_info(element_info::nested_block, &local.variablelist, block_tags::variable_list))
            ;

        local.variablelist =
                (cl::eps_p(*cl::blank_p >> cl::eol_p) | space)
            >>  local.table_title
            >>  *local.varlistentry
            ;

        local.varlistentry =
            space
            >>  cl::ch_p('[')
            >>  actions.values.list()
            [
                (
                    local.varlistterm
                    >>  (   +local.cell
                        |   cl::eps_p           [actions.error]
                        )
                    >>  cl::ch_p(']')
                    >>  space
                )
                | cl::eps_p                     [actions.error]
            ]
            ;

        local.varlistterm =
            space
            >>  cl::ch_p('[')
            >>  local.inner_phrase
            >>  (   cl::ch_p(']')
                >>  space
                |   cl::eps_p                   [actions.error]
                )
            ;

        elements.add
            ("table", element_info(element_info::nested_block, &local.table, block_tags::table))
            ;

        local.same_line = *cl::blank_p >> !(comment >> space);

        local.table =
                local.same_line
            >>  local.element_id_1_5
            >>  local.same_line
            >>  local.table_title
            >>  *local.table_row
            ;

        local.table_row =
            space
            >>  cl::ch_p('[')
            >>
            (
                (
                    actions.values.list(table_tags::row)
                    [   *local.cell
                    ]
                    >>  cl::ch_p(']')
                    >>  space
                )
                | cl::eps_p                     [actions.error]
            )
            ;

        local.table_title =
                qbk_before(106)
            >>  (*(cl::anychar_p - eol))        [actions.values.entry(ph::arg1, ph::arg2, table_tags::title)]
            >>  (+eol)
            |   qbk_since(106)
            >>  actions.to_value(table_tags::title)
                [
                    table_title_phrase
                ]
            >>  space
            ;

        elements.add
            ("ordered_list", element_info(element_info::nested_block, &local.list, block_tags::ordered_list, 106))
            ("itemized_list", element_info(element_info::nested_block, &local.list, block_tags::itemized_list, 106))
            ;

        local.list = *local.cell;

        local.cell =
                space
            >>  cl::ch_p('[')
            >>  (   local.inner_block
                >>  cl::ch_p(']')
                >>  space
                |   cl::eps_p                   [actions.error]
                )
            ;

        elements.add
            ("xinclude", element_info(element_info::conditional_or_block, &local.xinclude, block_tags::xinclude))
            ("import", element_info(element_info::conditional_or_block, &local.import, block_tags::import))
            ("include", element_info(element_info::conditional_or_block, &local.include, block_tags::include))
            ;

        local.xinclude =
               space
            >> local.include_filename
            ;

        local.import =
               space
            >> local.include_filename
            ;

        local.include =
               space
            >>
           !(
                ':'
                >> (*((cl::alnum_p | '_') - cl::space_p))
                                                [actions.values.entry(ph::arg1, ph::arg2, general_tags::include_id)]
                >> space
            )
            >> local.include_filename
            ;

        local.include_filename =
                qbk_before(106u)
            >>  (*(cl::anychar_p - phrase_end)) [actions.values.entry(ph::arg1, ph::arg2)]
            |   qbk_since(106u)
            >>  actions.to_value()
                [   *(  raw_escape
                    |   (cl::anychar_p - phrase_end)
                                                [actions.raw_char]
                    )
                ]
            ;

        local.inner_block =
            actions.to_value()
            [
                inside_paragraph
            ]
            ;

        local.inner_phrase =
            actions.to_value()
            [
                paragraph_phrase
            ]
            ;
    }
}
