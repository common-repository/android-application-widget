<?php 

define("COMMENT_QUERY","2A");
define("IMAGE_QUERY","5A");

class AndroidMarket {
	
	var $appName = "";
	var $cachePath = "";
	var $authString = "";
	var $App = null;
	var $AppImage = null;
	var $AppComments = null;
	var $AppJsPackage = null;

	var $debugMessage = "debuging";

	function init_session($username,$password,$query,$path,$ttl=3600) {

		$this->appName =  $query;
		$this->cachePath = $path;	
  
		$this->debugMessage = "classinit";

		if($this->needUpdate($ttl)){	
			
		 	

			$this->getAuthString($username,$password);

			$this->AppJsPackage = new AndroidMarketAppJsPackage();

			$apprequest = new AndroidMarketAppRequest();
			$apprequest->class_init($this->authString,$query);


			$this->App = new AndroidMarketApp();
			$this->App->class_init($this->getUnGziped($this->queryRequest($apprequest->requestStream)));
			$this->AppJsPackage->AppInfo = $this->App;
			$this->debugMessage = $this->authString;

			$image = new AndroidMarketRersourceRequest();
			$image->class_init($this->authString,$this->App->id,IMAGE_QUERY);
			$this->AppImage = new AndroidMarketImage();
			$this->AppImage->class_init($this->queryRequest($image->requestStream));
			$this->AppJsPackage->AppImage = base64_encode($this->AppImage->stream);


			$coments = new AndroidMarketRersourceRequest();
			$coments->class_init($this->authString,$this->App->id,COMMENT_QUERY);
			$this->AppComments = new AndroidMarketComments();
			$this->AppComments->class_init($this->getUnGziped($this->queryRequest($coments->requestStream)));
			$this->AppJsPackage->AppComments = $this->AppComments;

			$this->saveCacheData();

		}else{
		
			$this->getCachedData();
		}

 		
	}


	function needUpdate($ttl){

		

		$filename = $this->cachePath."/".$this->appName.".data";
		if(file_exists($filename)){
			if ((time()-filemtime($filename)) < $ttl){
				$this->debugMessage = "exist young";
				return false;

			}
			$this->debugMessage = "exist old";
			return true;
		}
		$this->debugMessage = "no exist";
		return true;

	}

	function getCachedData(){
		$filename = $this->cachePath."/".$this->appName.".data";
		$fh = fopen($filename,'r');
		$this->AppJsPackage = json_decode(fread($fh,filesize($filename)));
		fclose($fh);
		$this->App = $this->AppJsPackage->AppInfo;
		$this->AppImage->stream = base64_decode($this->AppJsPackage->AppImage);
		$this->AppComments = $this->AppJsPackage->AppComments;
	}
	
	function getAuthString($username,$password){

		$ch = curl_init();

		curl_setopt($ch, CURLOPT_URL, "https://www.google.com/accounts/ClientLogin");  
		curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);  
		  
		$data = array('Email' => $username,  
		'Passwd' => $password,  
		'accountType'=>'HOSTED_OR_GOOGLE',  
		'service'=>'android');  
		  
		curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, 0);  
		curl_setopt($ch, CURLOPT_POST, true);  
		curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);  
		curl_setopt($ch, CURLOPT_POSTFIELDS, $data);  
		  
		$hasil = curl_exec($ch);  
		$tok = strtok($hasil,"\r\n=");
		  
		$find = 0;

		while($tok !== false)
		{
			if($find == 1){
				$this->authString = $tok;
				break;
			}
			if(strtolower($tok) == "auth"){
				$find = 1;	
			}
			$tok = strtok("\r\n=");
		}

	}

	function queryRequest($RequestStream){
		$ch = curl_init();  
		  
		curl_setopt($ch, CURLOPT_URL, "http://android.clients.google.com/market/api/ApiRequest");  
		curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);  
		 




		$data = "version=2&request=".$RequestStream;  

		$headers[] = 'Cookie: ANDROID='.$this->authString;
		$headers[] = 'Content-Type: application/x-www-form-urlencoded';
		$headers[] = 'Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7';
		$headers[] = 'Accept: text/html, image/gif, image/jpeg, *; q=.2, */*; q=.2';
		$user_agent = 'Android-Market/2 (sapphire PLAT-RC33); gzip'; 
		  
		curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
		curl_setopt($ch, CURLOPT_HEADER, 0);
		curl_setopt($ch, CURLOPT_USERAGENT, $user_agent); 
		curl_setopt($ch, CURLOPT_POST, true);  
		curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);  
		curl_setopt($ch, CURLOPT_POSTFIELDS, $data);  

		return curl_exec($ch);

	}


	function getUnGziped($stream){
		$filename = $this->cachePath."/".$this->appName.".gz";
		$fh = fopen($filename,"w");
		fwrite($fh,$stream);
		fclose($fh);
		$zd = gzopen($filename,"r");
		$contents = gzread($zd, 10000);
		gzclose($zd);
		unlink($filename);
		return $contents;
	}

	function getJsonData(){
		return json_encode($this->AppJsPackage);
	}

	function saveCacheData(){
		$filename = $this->cachePath."/".$this->appName.".data";
		$fh = fopen($filename,"w");
		fwrite($fh,$this->getJsonData());
		fclose($fh);

		$filename = $this->cachePath."/".$this->appName.".png";
		$fh = fopen($filename,"w");
		fwrite($fh,$this->AppImage->stream);
		fclose($fh);
	}

	function h2s($value){
		return chr(hexdec($value));
	}
	
}
class AndroidMarketAppJsPackage {
	var $AppInfo = null;
	var $AppImage = null;
	var $AppComments = null;
}

class AndroidMarketAppRequest {
	
	var $requestStream = "";

	function class_init($auth,$query) {

		$request = $this->h2s('0A');
		$request .= $this->h2s("9D").$this->h2s('02');
		$request .= $this->h2s('0A');
		$request .= $this->h2s('CB').$this->h2s('01');
		$request .= $auth;
		$request .= $this->h2s('10').$this->h2s('00').$this->h2s('18').$this->h2s('EA').$this->h2s('07').$this->h2s('22').$this->h2s('10');
		$request .= "0000000000000000*";
		$request .= $this->h2s('0A');
		$request .= "sapphire:7";
		$request .= "2".$this->h2s('02');
		$request .= "es";
		$request .= ":".$this->h2s('02');
		$request .= "es";
		$request .= "B".$this->h2s('08');
		$request .= "T-Mobile";
		$request .= "J".$this->h2s('08');
		$request .= "T-Mobile";
		$request .= "R".$this->h2s('06');
		$request .= "310260";
		$request .= "Z".$this->h2s('06');
		$request .= "310260";
		$request .= $this->h2s('13').$this->h2s('22');
		$request .= chr(strlen($query)+ 8);
		$request .= $this->h2s('12');
		$request .= chr(strlen($query));
		$request .= $query;
		$request .= $this->h2s('30').$this->h2s('01').$this->h2s('40').$this->h2s('00').$this->h2s('48');
		$request .= $this->h2s('0A');
		$request .= $this->h2s('14');

		$this->requestStream = base64_encode($request);
	}
	
	function h2s($value){
		return chr(hexdec($value));
	}

}

class AndroidMarketRersourceRequest {
	
	var $requestStream = "";

	function class_init($auth,$appid,$magic) {

		$request = $this->h2s('0A');
		$request .= $this->h2s("9D").$this->h2s('02');
		$request .= $this->h2s('0A');
		$request .= $this->h2s('CB').$this->h2s('01');
		$request .= $auth;
		$request .= $this->h2s('10').$this->h2s('00').$this->h2s('18').$this->h2s('EA').$this->h2s('07').$this->h2s('22').$this->h2s('10');
		$request .= "0000000000000000*";
		$request .= $this->h2s('0A');
		$request .= "sapphire:7";
		$request .= "2".$this->h2s('02');
		$request .= "es";
		$request .= ":".$this->h2s('02');
		$request .= "es";
		$request .= "B".$this->h2s('08');
		$request .= "T-Mobile";
		$request .= "J".$this->h2s('08');
		$request .= "T-Mobile";
		$request .= "R".$this->h2s('06');
		$request .= "310260";
		$request .= "Z".$this->h2s('06');
		$request .= "310260";
		$request .= $this->h2s('13').$this->h2s($magic).$this->h2s('16');
		$request .= $this->h2s('0A');
		$request .= $this->h2s('14');
		$request .= $appid;
		$request .= $this->h2s('14');

		$this->requestStream = base64_encode($request);
	}

	function h2s($value){
		return chr(hexdec($value));
	}

}

class AndroidMarketApp{

	var $id = "";
	var $name = "";
	var $author = "";
	var $version = "";
	var $rating = "";
	var $description = "";
	var $permisions = "";
	var $packagename = "";
	var $category = "";
	var $mail = "";
	var $downloads = "";
	var $web = "";

	function class_init($contents) {
		$aux = hexdec(ord($contents[2])+1)-2;
		$this->id = substr($contents,$aux,20);

		$aux = $aux + 21;
		$len = ord(substr($contents,$aux,1));
		$aux = $aux + 1;
		$this->name = substr($contents,$aux,$len);

		$aux = $aux + $len+3;
		$len = ord(substr($contents,$aux,1));
		$aux = $aux + 1;
		$this->author = substr($contents,$aux,$len);

		$aux = $aux + $len+1;
		$len = ord(substr($contents,$aux,1));
		$aux = $aux + 1;
		$this->version = substr($contents,$aux,$len);

		$aux = $aux + $len+1;
		$len = ord(substr($contents,$aux,1));
		$aux = $aux + 1;
		$this->rating = substr($contents,$aux,$len);


		$aux = $aux + $len +6;
		$find = 0;
		$desc = "";
		while($find <3){

			$mychar = substr($contents,$aux,1);
			$desc .= $mychar;
			if(ord($mychar) == 112){
				$find = $find +1;
			}else if(ord($mychar) == 0){
				$find = $find +1;
			}else if(ord($mychar) == 122){
				$find = $find +1;
			}else{
				$find = 0;
			}
			$aux = $aux + 1;
		}
		$this->description = preg_replace('/[^[:print:]]+/','',htmlentities(substr($desc,0,strlen($desc)-3),ENT_COMPAT,'UTF-8'));

		$moreper = 1;
		$permisionlist = array();
		$counter = 0;
		while($moreper > 0){
			$len = ord(substr($contents,$aux,1));
			$aux = $aux + 1;
			$permisionlist[$counter] = substr($contents,$aux,$len);
			$counter++;
			$aux = $aux + $len;

			if(substr($contents,$aux,1) != 'z'){
				$moreper = 0;
			}else{
				$aux = $aux + 1;
			}
		}
		$this->permisions = $permisionlist;


		$aux = $aux + 7;
		$len = ord(substr($contents,$aux,1));
		$aux = $aux + 1;
		$this->packagename = substr($contents,$aux,$len);
		$pkname = substr($contents,$aux,$len);

		$aux = $aux + $len + 2;
		$len = ord(substr($contents,$aux,1));
		$aux = $aux + 1;
		$this->category = substr($contents,$aux,$len);

		$aux = $aux + $len + 5;
		$len = ord(substr($contents,$aux,1));
		$aux = $aux + 1;
		$this->mail = substr($contents,$aux,$len);

		$aux = $aux + $len + 2;
		$len = ord(substr($contents,$aux,1));
		$aux = $aux + 1;
		$this->downloads = substr($contents,$aux,$len);

		$aux = $aux + $len + 2;
		$len = ord(substr($contents,$aux,1));
		$aux = $aux + 1;
		$this->web = substr($contents,$aux,$len);
	
	}
}


class AndroidMarketImage{
	var $stream = "";

	function class_init($stream) {
		$strstart = stripos($stream,"PNG")-1;
		$strstartend = stripos($stream,"IEND")+8;
		$this->stream = substr($stream,$strstart,($strstartend-$strstart));
	}

	function saveToFile($filename){
		$fh = fopen($filename.".png","w");
		fwrite($fh,$this->stream);
		fclose($fh);
	}

}

class AndroidMarketComments{
	var $stream = "";
	var $comments = array();

	function class_init($stream) {
		$this->stream = $stream;
		$aux = 29;
		$finish = 0;
		$stremalength = strlen($stream);
		$count = 0;
		while($finish <= 0){
			$comment = new AndroidMarketComment();
			$len = ord($stream[$aux]);
			$aux++;
			$comment->comment = substr($stream,$aux,$len);
			$aux = $aux+ $len + 1;
			$comment->score = ord(substr($stream,$aux,1));
			$aux = $aux+ 2;
			$len = ord($stream[$aux]);
			$aux++;
			$comment->name = substr($stream,$aux,$len);
			$aux = $aux+ $len;
			$aux++;
			$comment->date =  $this->dateToUnix(substr($stream,$aux,6));
			$aux = $aux+ 30;
			$this->comments[$count] = $comment;
			$count++;
			if($aux >= ($stremalength-5)){
				$finish = 1;
			}else{
				if(ord($stream[$aux]) == 10){
				$aux = $aux+ 1;
				}else{
				$aux = $aux+ 2;
				}
			}
		}
	}

	function saveToFile($filename){
		$fh = fopen($filename."_coments.dat","w");
		fwrite($fh,$this->stream);
		fclose($fh);
	}

	function dateToUnix($string){
		$aux = "";
		for($i = 0; $i < 6; $i++){
		  //$aux = ord(substr($string,$i,1))."-".$aux;
		  $aux = substr($this->dectobin(ord(substr($string,$i,1))),1).$aux;
		}
		return round(bindec($aux)/1000);
	}
	function dectobin($value){
		$aux = decbin($value);
		$len = strlen($aux);
		for($i = $len; $i < 8;$i++){
		   $aux = "0".$aux;
		}
		return $aux;
	}
}

class AndroidMarketComment{

	var $comment = "";
	var $score = 0;
	var $name = "";
	var $date = "";
}
?>
