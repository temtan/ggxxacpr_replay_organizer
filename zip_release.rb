
require './ttrl/releaser.rb'

base = File.dirname( File.expand_path( __FILE__ ) )
project_name = "ggxxacpr_replay_organizer"

version = lambda {
  File.read( "./common.h" ).each_line{|line|
    if ( /VERSION/.match( line ) )
      return /\"([^\"]*)\"/.match( line )[1]
    end
  }
}.call


## -- exe -----
STDERR.puts( "== exe ====================" )

releaser = TTRL::Releaser.new( project_name, base )
releaser.strftime_template = ''
releaser.additional_string = "-#{version}"

[
"Release/ggxxacpr_replay_organizer.exe",
"ggxxacpr_replay_organizer.txt",
].each {|file| releaser.add_file( file ) }

releaser.add_pre_copy {|file_utils, to_base|
  [
  ].each {|one| file_utils.mkdir( "#{to_base}/#{one}" ) }

  file_utils.mkdir( "#{to_base}/lang" )
  file_utils.cp( "./Release/lang/japanese.dll",            "#{to_base}/lang" )
  file_utils.cp( "./Release/lang/english.dll",             "#{to_base}/lang" )
  file_utils.cp( "./Release/lang/chinese_traditional.dll", "#{to_base}/lang" )
  file_utils.cp( "./Release/lang/korean.dll",              "#{to_base}/lang" )
}

releaser.release_dir = "../backups/#{project_name}/web"
releaser.make_zip
