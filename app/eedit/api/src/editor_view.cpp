#include <map>
#include <memory>

#include "handle_table.h"

#include "editor_types.h"
#include "editor_view.h"
#include "editor_buffer.h"
#include "codepoint_info.h"

#include "codec.h"


#include "../../core/input/event/input_event_map.hpp" // FIXME: typedef uint64_t editor_input_event_map_id_t;

////////////////////////////////////////////////////////////////////////////////

struct editor_view {
	editor_view(editor_buffer_id_t editor_buffer_id_);
	~editor_view();

	editor_buffer_id_t editor_buffer_id = 0;
	editor_view_id_t   view = 0;
	codec_id_t codec_id;

	bool region_changed   = false;
	bool ui_need_refresh  = false;
	bool ui_must_resync   = false;

	uint64_t start_offset = 0; // merge with codepoint_info ?
	uint64_t end_offset   = 0;

	codepoint_info_t start_cpi;

	std::map<std::string, eedit::editor_input_event_map *> input_event_table; // FIXME

};


/// local table
static handle_index_allocator<struct editor_view> table;

editor_view::editor_view(editor_buffer_id_t editor_buffer_id_)
	: editor_buffer_id(editor_buffer_id_)
{
	app_log << "loading input map configuration\n";
	auto bret = eedit::setup_default_input_map(input_event_table);
	assert(bret == true);

	// must remove this ? put split cotext in text-mode
	start_cpi.codepoint = -1;
	start_cpi.offset    = 0;
	start_cpi.used      = 0;

	app_log << "set default text codec\n";
	//
	codec_id = codec_get_by_name("text/ascii");

	//    setup_default_input_map(input_event_table);
}

editor_view::~editor_view()
{
	eedit::release_input_map(input_event_table);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {

	SHOW_SYMBOL
	editor_view_id_t editor_view_open(editor_buffer_id_t editor_buffer_id)
	{
		editor_view * vptr;
		editor_view_id_t view;
		std::tie(vptr, view) = table.construct(editor_buffer_id);
		if (!view) {
			return INVALID_EDITOR_VIEW_ID;
		}

		vptr->view = view;

		app_log << __PRETTY_FUNCTION__ << " allocated view   = " << view << "\n";
		return view;
	}

	SHOW_SYMBOL
	int editor_view_close(editor_view_id_t view_id)
	{
		return 0;
	}

	SHOW_SYMBOL
	uint64_t editor_view_get_start_offset(editor_view_id_t view)
	{
		return 0;
	}

	SHOW_SYMBOL
	uint64_t editor_view_set_start_offset(editor_view_id_t view, uint64_t offset)
	{
		return 0;
	}


	SHOW_SYMBOL
	uint64_t editor_view_get_end_offset(editor_view_id_t view)
	{
		return 0;
	}


	SHOW_SYMBOL
	uint64_t editor_view_set_end_offset(editor_view_id_t view, uint64_t offset)
	{
		return 0;
	}



// view -> codec
	SHOW_SYMBOL
	codec_id_t         editor_view_get_codec_id(editor_view_id_t  view)
	{
		assert(0);
	}

	SHOW_SYMBOL
	codec_context_id_t editor_view_get_codec_ctx(editor_view_id_t view)
	{
		assert(0);
	}


	SHOW_SYMBOL
	editor_font_t editor_view_get_font(editor_view_id_t view)
	{
		assert(0);
		return 0;
	}


	SHOW_SYMBOL
	void editor_view_set_region_changed_flag(editor_view_id_t view, bool flag)
	{
		auto v = table.get(view);
		if (v == nullptr)
			return;

		v->region_changed = flag;
	}

	SHOW_SYMBOL
	bool editor_view_get_region_changed_flag(editor_view_id_t view)
	{
		auto v = table.get(view);
		if (v == nullptr)
			return false;

		return v->region_changed;
	}

	SHOW_SYMBOL
	void editor_view_set_ui_need_refresh(editor_view_id_t view, bool flag)
	{
		auto v = table.get(view);
		if (v == nullptr)
			return;

		v->ui_need_refresh = flag;
	}

	SHOW_SYMBOL
	bool editor_view_get_ui_need_refresh(editor_view_id_t view)
	{
		auto v = table.get(view);
		if (v == nullptr)
			return false;

		return v->ui_need_refresh;
	}



	SHOW_SYMBOL
	void editor_view_set_ui_must_resync(editor_view_id_t view, bool flag)
	{
		auto v = table.get(view);
		if (v == nullptr)
			return;

		v->ui_must_resync = flag;
	}

	SHOW_SYMBOL
	bool editor_view_get_ui_must_resync(editor_view_id_t view)
	{
		auto v = table.get(view);
		if (v == nullptr)
			return false;

		return v->ui_need_refresh;
	}

	SHOW_SYMBOL
	void editor_view_set_start_cpi(editor_view_id_t view, const codepoint_info_t * cpi)
	{
		auto v = table.get(view);
		if (v == nullptr)
			return;

		memcpy(&v->start_cpi, cpi, sizeof (codepoint_info_t));
	}

	SHOW_SYMBOL
	void editor_view_get_start_cpi(editor_view_id_t view, codepoint_info_t * cpi)
	{
		auto v = table.get(view);
		if (v == nullptr)
			return;

		memcpy(cpi, &v->start_cpi, sizeof (codepoint_info_t));
	}



} // ! extern "C"
