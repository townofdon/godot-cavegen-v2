class_name SideToolbar
extends Panel

@onready var mode_room_select: TextureButton = $VBoxContainer/ModeRoomSelect
@onready var mode_room_config: TextureButton = $VBoxContainer/ModeRoomConfig
@onready var mode_tile_editor: TextureButton = $VBoxContainer/ModeTileEditor
@onready var mode_preview: TextureButton = $VBoxContainer/ModePreview

const texture_sprite_size:int = 24
enum AtlasTextureX {
	RoomSelect = 0,
	RoomSelectActive = 1,
	RoomConfig = 6,
	RoomConfigActive = 7,
	TileEditor = 12,
	TileEditorActive = 13,
	Preview = 10,
	PreviewActive = 11,
}

var cavegen: CaveGen

func initialize(p_cavegen: CaveGen):
	cavegen = p_cavegen
	if !cavegen.mode_changed.is_connected(_on_mode_changed):
		cavegen.mode_changed.connect(_on_mode_changed)

func _ready() -> void:
	mode_room_select.pressed.connect(func(): cavegen.set_mode(CaveGen.Mode.RoomSelect))
	mode_room_config.pressed.connect(func(): cavegen.set_mode(CaveGen.Mode.RoomConfig))
	mode_tile_editor.pressed.connect(func(): cavegen.set_mode(CaveGen.Mode.TileEditor))
	mode_preview.pressed.connect(func(): cavegen.set_mode(CaveGen.Mode.Preview))
	_on_mode_changed(cavegen.mode)

func _on_mode_changed(mode: CaveGen.Mode) -> void:
	_set_btn_texture(mode_room_select, AtlasTextureX.RoomSelect)
	_set_btn_texture(mode_room_config, AtlasTextureX.RoomConfig)
	_set_btn_texture(mode_tile_editor, AtlasTextureX.TileEditor)
	_set_btn_texture(mode_preview, AtlasTextureX.Preview)
	if mode == CaveGen.Mode.RoomSelect:
		_set_btn_texture(mode_room_select, AtlasTextureX.RoomSelectActive)
	elif mode == CaveGen.Mode.RoomConfig:
		_set_btn_texture(mode_room_config, AtlasTextureX.RoomConfigActive)
	elif mode == CaveGen.Mode.TileEditor:
		_set_btn_texture(mode_tile_editor, AtlasTextureX.TileEditorActive)
	elif mode == CaveGen.Mode.Preview:
		_set_btn_texture(mode_preview, AtlasTextureX.PreviewActive)

func _set_btn_texture(button: TextureButton, texture_x: AtlasTextureX) -> void:
	var texture:AtlasTexture = button.texture_normal
	assert(texture is AtlasTexture)
	texture.region.position.x = texture_x * texture_sprite_size
