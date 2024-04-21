// ggxxacpr_replay_organizer.tdp.nut

local register_lang_dll = function( lang ) {
  {
    local tmp = MakeCommandEntry( Developer.current_structure.output_directory_name + "\\" + lang + ".res" );
    tmp.included_in_build = true;
    tmp.depends.append( "lang\\" + lang + ".rc" );
    tmp.commands.append( "$(RC) /nologo -Fo$@ $?" );
    Developer.current_structure.additional_make_commands.append( tmp );
  }
  {
    local tmp = MakeCommandEntry( Developer.current_structure.output_directory_name + "\\lang\\" + lang + ".dll" );
    tmp.included_in_build = true;
    tmp.depends.append( Developer.current_structure.output_directory_name + "\\" + lang + ".res" );
    tmp.commands.append( "if not exist " + Developer.current_structure.output_directory_name + "\\lang mkdir " + Developer.current_structure.output_directory_name + "\\lang" );
    tmp.commands.append( "$(LINK) /DLL /NOENTRY /MACHINE:X64 $? /OUT:$@" );
    Developer.current_structure.additional_make_commands.append( tmp );
  }
};
register_lang_dll( "japanese" );
register_lang_dll( "english" );
register_lang_dll( "chinese_traditional" );
register_lang_dll( "korean" );

{
  local tmp = MakeCommandEntry( "lang_clean" );
  tmp.included_in_clean = true;
  local register_tmp = function( lang ) {
    tmp.commands.append( "del " + Developer.current_structure.output_directory_name + "\\" + lang + ".res" );
    tmp.commands.append( "del " + Developer.current_structure.output_directory_name + "\\lang\\" + lang + ".dll" );
  };
  register_tmp( "japanese" );
  register_tmp( "english" );
  register_tmp( "chinese_traditional" );
  register_tmp( "korean" );
  Developer.current_structure.additional_make_commands.append( tmp );
}
