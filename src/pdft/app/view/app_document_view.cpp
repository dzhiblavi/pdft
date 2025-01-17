#include "pdft/app/view/impl/app_document_view.h"

#include "platform/module/register.h"

namespace pdft::app {

tui::Element AppDocumentView::getLayout(
    platform::trace::Context& /*ctx*/, const DocumentModel& doc) {
    switch (doc.view_mode) {
        using enum ViewMode;

        case OnePage:
            return tui::empty(tl);

        case TwoPages:
            return tui::hbox(tui::empty(tl) | tui::flex, tui::empty(tr) | tui::flex);

        case OnePageContinuous:
            return tui::vbox({
                       tui::empty(tl) | tui::flex |
                           tui::size(
                               tui::HEIGHT, tui::EQUAL, UI::ScrollPerPage - doc.scroll_offset),
                       tui::empty(bl) | tui::flex |
                           tui::size(tui::HEIGHT, tui::EQUAL, doc.scroll_offset),
                   }) |
                   tui::reflect(rect);

        case TwoPagesContinuous:
            return tui::vbox({
                       tui::hbox({
                           tui::empty(tl) | tui::flex |
                               tui::size(
                                   tui::HEIGHT, tui::EQUAL, UI::ScrollPerPage - doc.scroll_offset),
                           tui::empty(tr) | tui::flex |
                               tui::size(
                                   tui::HEIGHT, tui::EQUAL, UI::ScrollPerPage - doc.scroll_offset),
                       }) | tui::flex,
                       tui::hbox({
                           tui::empty(bl) | tui::flex |
                               tui::size(tui::HEIGHT, tui::EQUAL, doc.scroll_offset),
                           tui::empty(br) | tui::flex |
                               tui::size(tui::HEIGHT, tui::EQUAL, doc.scroll_offset),
                       }) | tui::flex,
                   }) |
                   tui::reflect(rect);
    }
}

platform::Async<void> AppDocumentView::render(
    platform::trace::Context& ctx,
    const DocumentModel& doc,
    const DisplayProperties& properties,
    tui::Screen& screen) const {
    auto _ = ctx.push("docview:render");

    switch (doc.view_mode) {
        using enum ViewMode;

        case OnePage:
            co_return co_await renderOnePage(ctx, doc, properties, screen);

        case TwoPages:
            co_return co_await renderTwoPages(ctx, doc, properties, screen);

        case OnePageContinuous:
            co_return co_await renderOnePageContinuous(ctx, doc, properties, screen);

        case TwoPagesContinuous:
            co_return co_await renderTwoPagesContinuous(ctx, doc, properties, screen);
    }
}

platform::Async<void> AppDocumentView::renderOnePage(
    platform::trace::Context& ctx,
    const DocumentModel& doc,
    const DisplayProperties& properties,
    tui::Screen& screen) const {
    auto _ = ctx.push("docview:page");

    co_await renderer_->render(
        ctx,
        {
            .document_id = doc.document_id,
            .page_index = doc.page_index,
        },
        {
            .rect = tl,
            .placement =
                {
                    .hor = tui::Alignment::Center,
                    .vert = tui::Alignment::Center,
                },
        },
        properties,
        screen);
}

platform::Async<void> AppDocumentView::renderTwoPages(
    platform::trace::Context& ctx,
    const DocumentModel& doc,
    const DisplayProperties& properties,
    tui::Screen& screen) const {
    ctx.push("docview:left") |
        co_await renderer_->render(
            ctx,
            {
                .document_id = doc.document_id,
                .page_index = doc.page_index,
            },
            {
                .rect = tl,
                .placement =
                    {
                        .hor = tui::Alignment::Max,
                        .vert = tui::Alignment::Center,
                    },
            },
            properties,
            screen);

    if (doc.page_index + 1 >= doc.page_count) {
        co_return;
    }

    ctx.push("docview:right") |
        co_await renderer_->render(
            ctx,
            {
                .document_id = doc.document_id,
                .page_index = doc.page_index + 1,
            },
            {
                .rect = tr,
                .placement =
                    {
                        .hor = tui::Alignment::Min,
                        .vert = tui::Alignment::Center,
                    },
            },
            properties,
            screen);
}

platform::Async<void> AppDocumentView::renderOnePageContinuous(
    platform::trace::Context& ctx,
    const DocumentModel& doc,
    const DisplayProperties& properties,
    tui::Screen& screen) const {
    float tl_ratio = float(tl.height) / float(rect.height);

    ctx.push("docview:top") |
        co_await renderer_->render(
            ctx,
            {
                .document_id = doc.document_id,
                .page_index = doc.page_index,
            },
            {
                .rect = tl,
                .subarea =
                    {
                        .x = 0.f,
                        .y = 1.f - tl_ratio,
                        .width = 1.f,
                        .height = tl_ratio,
                    },
                .placement =
                    {
                        .hor = tui::Alignment::Center,
                        .vert = tui::Alignment::Max,
                    },
            },
            properties,
            screen);

    if (doc.page_index + 1 >= doc.page_count) {
        co_return;
    }

    float bl_ratio = float(bl.height) / float(rect.height);

    ctx.push("docview:bottom") |
        co_await renderer_->render(
            ctx,
            {
                .document_id = doc.document_id,
                .page_index = doc.page_index + 1,
            },
            {
                .rect = bl,
                .subarea =
                    {
                        .x = 0.f,
                        .y = 0.f,
                        .width = 1.f,
                        .height = bl_ratio,
                    },
                .placement =
                    {
                        .hor = tui::Alignment::Center,
                        .vert = tui::Alignment::Min,
                    },
            },
            properties,
            screen);
}

platform::Async<void> AppDocumentView::renderTwoPagesContinuous(
    platform::trace::Context& ctx,
    const DocumentModel& doc,
    const DisplayProperties& properties,
    tui::Screen& screen) const {
    float tl_ratio = float(tl.height) / float(rect.height);

    ctx.push("docview:top_left") |
        co_await renderer_->render(
            ctx,
            {
                .document_id = doc.document_id,
                .page_index = doc.page_index,
            },
            {
                .rect = tl,
                .subarea =
                    {
                        .x = 0.f,
                        .y = 1.f - tl_ratio,
                        .width = 1.f,
                        .height = tl_ratio,
                    },
                .placement =
                    {
                        .hor = tui::Alignment::Max,
                        .vert = tui::Alignment::Max,
                    },
            },
            properties,
            screen);

    if (doc.page_index + 1 >= doc.page_count) {
        co_return;
    }

    float tr_ratio = float(tr.height) / float(rect.height);

    ctx.push("docview:top_right") |
        co_await renderer_->render(
            ctx,
            {
                .document_id = doc.document_id,
                .page_index = doc.page_index + 1,
            },
            {
                .rect = tr,
                .subarea =
                    {
                        .x = 0.f,
                        .y = 1.f - tr_ratio,
                        .width = 1.f,
                        .height = tr_ratio,
                    },
                .placement =
                    {
                        .hor = tui::Alignment::Min,
                        .vert = tui::Alignment::Max,
                    },
            },
            properties,
            screen);

    if (doc.page_index + 2 >= doc.page_count) {
        co_return;
    }

    float bl_ratio = float(bl.height) / float(rect.height);

    ctx.push("docview:bottom_left") |
        co_await renderer_->render(
            ctx,
            {
                .document_id = doc.document_id,
                .page_index = doc.page_index + 2,
            },
            {
                .rect = bl,
                .subarea =
                    {
                        .x = 0.f,
                        .y = 0.f,
                        .width = 1.f,
                        .height = bl_ratio,
                    },
                .placement =
                    {
                        .hor = tui::Alignment::Max,
                        .vert = tui::Alignment::Min,
                    },
            },
            properties,
            screen);

    if (doc.page_index + 3 >= doc.page_count) {
        co_return;
    }

    float br_ratio = float(br.height) / float(rect.height);

    ctx.push("docview:bottom_right") |
        co_await renderer_->render(
            ctx,
            {
                .document_id = doc.document_id,
                .page_index = doc.page_index + 3,
            },
            {
                .rect = br,
                .subarea =
                    {
                        .x = 0.f,
                        .y = 0.f,
                        .width = 1.f,
                        .height = br_ratio,
                    },
                .placement =
                    {
                        .hor = tui::Alignment::Min,
                        .vert = tui::Alignment::Min,
                    },
            },
            properties,
            screen);
}

}  // namespace pdft::app

PLATFORM_REGISTER_MODULE(pdft::app::AppDocumentView);
