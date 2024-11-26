#include "pdft/view/app/renderer.h"

#include "tests/pdft/image/mock_image.h"
#include "tests/pdft/media/document.h"
#include "tests/pdft/media/page.h"
#include "tests/pdft/run_coroutine.h"
#include "tests/pdft/tui/display/mock_display.h"

#include <catch2/catch_test_macros.hpp>

namespace pdft::view::app {

using namespace testing;

const core::terminal::Dimensions TermSize = {
    .rows = 40,
    .columns = 200,
    .height_px = 1080,
    .width_px = 1920,
    .cell_height_px = 1080 / 40,
    .cell_width_px = 1920 / 200,
};

const tui::IRect Rect = {
    .x = 10,
    .y = 20,
    .width = 30,
    .height = 40,
};

const PagePlacement Placement = {};

asio::awaitable<void> just() {
    co_return;
}

struct TestRenderer {
    TestRenderer() {
        EXPECT_CALL(*doc, getPath()).WillOnce(ReturnRef(path));
        doc_id = docs->addDocument(doc);
    }

    std::filesystem::path path = "path.pdf";
    std::shared_ptr<media::mock::Document> doc = std::make_shared<media::mock::Document>();
    std::shared_ptr<media::mock::Page> page = std::make_shared<media::mock::Page>();
    std::shared_ptr<state::DocumentStorage> docs = std::make_shared<state::DocumentStorage>();
    std::shared_ptr<tui::display::mock::Display> display =
        std::make_shared<tui::display::mock::Display>();
    std::shared_ptr<image::mock::Image> image = std::make_shared<image::mock::Image>();
    state::DocumentId doc_id;
    core::io::OutBuffer buffer;
    Renderer renderer = Renderer(TermSize, docs, display, {});
};

TEST_CASE_METHOD(TestRenderer, "testRendererRenderSamePage", "") {
    tests::runCoroutine([this](auto&) -> asio::awaitable<void> {
        uint32_t page_index = 1;

        // Load a page for the first time
        {
            testing::InSequence seq;
            EXPECT_CALL(*doc, getPage(page_index)).WillOnce(Return(page));

            EXPECT_CALL(*page, getDefaultSize())
                .WillOnce(Return(media::PageSize{
                    .width_px = 100,
                    .height_px = 100,
                }));
            EXPECT_CALL(
                *page,
                getImage(display::ImageProperties{
                    .scaling_factor = 10.8f,
                    .invert_colors = false,
                }))
                .WillOnce(Return(image));

            EXPECT_CALL(*display, loadImage(_, _, _)).WillOnce(Return(just()));
            EXPECT_CALL(*image, size())
                .WillOnce(Return(display::ImageSize{
                    .width_px = 10,
                    .height_px = 10,
                }));
            EXPECT_CALL(*display, isLoaded(_)).WillOnce(Return(true));
            EXPECT_CALL(*display, displayImage(_, _, _, _)).WillOnce(Return(just()));

            co_await renderer.render(
                buffer,
                Rect,
                Placement,
                display::ImageProperties{},
                {
                    .document_id = doc_id,
                    .page_index = page_index,
                });
        }

        // Display an already loaded image
        {
            testing::InSequence seq;
            EXPECT_CALL(*display, isLoaded(_)).WillOnce(Return(true));
            EXPECT_CALL(*display, displayImage(_, _, _, _)).WillOnce(Return(just()));

            co_await renderer.render(
                buffer,
                Rect,
                Placement,
                display::ImageProperties{},
                {
                    .document_id = doc_id,
                    .page_index = page_index,
                });
        }
    });
}

}  // namespace pdft::view::app
