<?php
/*
Plugin Name: Android Application Widget
Plugin URI: http://www.ikearlbeniz.net/
Description: A WordPress widget that displays an Android application info updated fed by Android Market.
Version: 1.0
Author: Iker Perez de Albeniz
Author URI: http://www.ikeralbeniz.net
License: GPL (http://www.gnu.org/copyleft/gpl.html)
Notes: Requires at least PHP 4 and Curl. Options for the widget are gmail username, paswrod and application name. Template functions are provided for customizability.
*/

/* Version 1.0 - initial release
 * March 30, 2010
 *
 */

include_once WP_PLUGIN_DIR."/android-app-widget/AndroidMarket.inc";

function widget_andapp() {
	global $gusername;
	if (!$options = get_option('Android Application Widget'))
		$options = array('gusername' => '@gmail.com', 'gpassword' => '','appname' => '');
	$gusername = $options['gusername'];
	$gpassword = $options['gpassword'];
	$appname = $options['appname'];
	?><div class="widget"><?php	
	android_draw_widget($gusername,$gpassword,$appname);
	?></div><?php
}

function android_draw_widget($gusername,$gpassword,$appname,$ttl=3600){
	
	$market = new AndroidMarket();
	$market->init_session($gusername,$gpassword,$appname,WP_PLUGIN_DIR.'/android-app-widget/cache',$ttl);
	?>
	<!-- aMarket widget -->
	<div id="wpmarket">
	<!-- aMarket widget titlebar -->
	<div id="wpmarket_apptitlebar">
		<div id="wpmarket_apptitle">
			<img src="<?php echo WP_PLUGIN_URL.'/android-app-widget/cache/'.$appname; ?>.png" class="wpmarket_applogo" title="<?php echo $appname; ?> Application Logo" alt="<?php echo $appname; ?> Application Logo">
			<span class="wpmarket_appname"><?php echo $market->App->name;?></span><br>
			<span class="wpmarket_author"><?php echo $market->App->author; ?></span>
		</div>
		<div id="wpmarket_appscore" style="float:right;width:65px;margin-top:-25px;">
			<!--<div id="wpmarket_pricebox" style="text-align:right;width:60px;padding-right:5px;"><span class="wpmarket_price" style="font-size:14px;font-weight: bold;margin-bottom:10px;">FREE</span></div>-->
			<span class="wpmarket_starsbg"><span class="wpmarket_stars" style="width:<?php echo round($market->App->rating *12,0)?>px;"></span></span>
			
		</div>
	</div>
	<!-- aMarket widget titlebar END-->
	<!-- aMarket widget installcodebar -->
		<div id="wpmarket_installcodebar">
			<div id="wpmarket_installcodebar_close" >
				<input type="button" value=" " onClick="javascript:document.getElementById('wpmarket_installcodebar').style.visibility='hidden';">
			</div>
			<div id="wpmarket_installcodebar_image" >
				<img src="http://chart.apis.google.com/chart?cht=qr&chs=135x135&chl=market://search?q=pname:<?php echo $market->App->packagename;?>">
			</div>
		</div>
	<!-- aMarket widget installcodebar END-->
	<!-- aMarket widget content -->
		<div id="wpmarket_contentbox">
		<!-- scroll divs -->
		<div id="scrollholder" class="scrollholder">
        <div id="scroll" class="scroll">
    <!-- scroll divs -->
		<div id="wpmarket_separator"></div>
		<div id="wpmarket_appsubtitlebar">
			<span id="wpmarket_appsubtitle"><?php echo $market->App->downloads; ?> <?php echo __('downloads') ?></span>
		</div>
		<div id="wpmarket_separator"></div>
		
		<div id="wpmarket_appblock">
			<span class="wpmarket_desription">

			<?php echo  preg_replace('/[^[:print:]]+/','',preg_replace('/(\n+)/i', '<br/><br/>', $market->App->description)); ?> 
			</span>
			<br><br>
			<span class="wpmarket_version">
			version: <?php echo $market->App->version; ?>
			</span>
		</div>
		
		<div id="wpmarket_separator"></div>
		<div id="wpmarket_appsubtitlebar">
			<span id="wpmarket_appsubtitle"><?php echo __('Comments') ?></span>
		</div>
		<div id="wpmarket_separator"></div>
		
		<div id="wpmarket_commentblock">
			<?php
			 $nocomment = 0;
			 foreach($market->AppComments->comments as $comment){
			 $nocomment++;
			?>
			<div id="wpmarket_comment">
			<div id="wpmarket_comentauthor">
				<?php echo $comment->name; ?> <span class="wpmarket_commentdate"><?php echo date("Y-m-d",$comment->date); ?></span>
			</div>
			<div id="wpmarket_commentscore">
				<span class="wpmarket_coment_starsbg"><span class="wpmarket_coment_stars" style="width:<?php echo round($comment->score *12,0)?>px;"></span></span>
			</div>
			<span id="wpmarket_commenttext">
			<?php echo $comment->comment; ?>
			</span>
			</div>
			<div id="wpmarket_comment_separator"></div>
			<?php
				}
			 if($nocomment <= 0){
			?>
			<div id="wpmarket_comment">
			<span id="wpmarket_commenttext">
			<?php echo __('No Comments Available') ?>
			</span>
			</div>
			<div id="wpmarket_comment_separator"></div>
			<?php
			 }
			?>
					
		<!-- scroll divs -->
		</div>
		</div>
		<script type="text/javascript">
        ScrollLoad ("scrollholder", "scroll", true);
    </script>
		<!-- scroll divs -->
		</div>
		
	<!-- aMarket widget content END-->
	<!-- aMarket widget installbutton -->		
		<div id="wpmarket_apptitlebar">
		<input type="button" value="<?php echo __('Install') ?>" class="wpmarket_install_button" onclick="javascript:document.getElementById('wpmarket_installcodebar').style.visibility='visible';">
		</div>
	<!-- aMarket widget installbutton END-->
	<div id="wpmarket_credits"><span class="wpmarket_powered">prowered by <a href="http://ikeralbeniz.net">iker</a></span></div>
</div>
</div>
<!-- aMarket widget END-->

	<?php
}

/* The code for the widget options */
function widget_andapp_options() {
	if(!$options = get_option('Android Application Widget'))
		$options = array('gusername' => '@gmail.com', 'gpassword' => '','appname' => '');
	
	if($_POST['andapp-submit']) {
		$options = array('gusername' => $_POST['andapp-username'], 'gpassword' => $_POST['andapp-password'], 'appname' => $_POST['andapp-appname']);
		update_option('Android Application Widget', $options);
	}
?>
	<p><?php echo __('Username')?> (google):<br><input type="text" style="width:220px;" id="twitter-username" name="andapp-username" value="<?php echo $options['gusername']; ?>" /></p>
	<p><?php echo __('Password')?>:<br><input type="password" style="width:220px;" id="andapp-password" name="andapp-password" value="<?php echo $options['gpassword']; ?>" /></p>
	<p><?php echo __('App Name')?>:<br><input type="text" style="width:220px;" id="andapp-appname" name="andapp-appname" value="<?php echo $options['appname']; ?>" /></p>
	<input type="hidden" id="andapp-submit" name="andapp-submit" value="1" />
<?php
}

 function add_my_stylesheet() {
        $myStyleUrl = WP_PLUGIN_URL . '/android-app-widget/css/andapp.css';
        $myStyleFile = WP_PLUGIN_DIR . '/android-app-widget/css/andapp.css';
        if ( file_exists($myStyleFile) ) {
            wp_register_style('appandroid-widget', $myStyleUrl);
            wp_enqueue_style( 'appandroid-widget');
        }
    }


function andapp_init() {
	register_sidebar_widget(__('Android Application Widget'), 'widget_andapp');
	register_widget_control(__('Android Application Widget'), 'widget_andapp_options', 200, 200);
	wp_register_script('appandroid-widget', WP_PLUGIN_URL . '/android-app-widget/js/scroll.js');
	wp_enqueue_script('appandroid-widget');


}

add_action('plugins_loaded', 'andapp_init');
add_action('wp_print_styles', 'add_my_stylesheet');


?>
