download_builds() {
  # cleanup
  mkdir -p downloads/
  rm -rf downloads/*

  source_url="$1"
  target_url="$2"
  source_http_user="$3"
  source_http_password="$4"
  target_http_user="$5"
  target_http_password="$6"

  if [ -z "$source_url" ] || [ -z "$target_url" ]
  then
    "download_builds usage: <source_url> <target_url>"
    exit 1
  fi

  download_file "$source_url" "$source_http_user" "$source_http_password"
  download_file "$target_url" "$target_http_user" "$target_http_password"
}

download_file() {
  PARAMS="-nv"
  url="$1"
  http_user="$2"
  http_password="$3"

  if [ -z "$url" ]
  then
    "download_file usage: <url> <username> <password>"
    exit 1
  fi

  source_file=`basename "$url"`
  cd downloads 
  if [ -f "$source_file" ]; then rm "$source_file"; fi
  if [ ! -z "$http_user" ] && [ ! -z "$http_password" ]
  then
    wget --no-check-certificate $PARAMS --user="$http_user" --password="$http_password" "$url" 2>&1
  else
    wget --no-check-certificate $PARAMS "$url" 2>&1
  fi
  if [ $? != 0 ]; then
    echo "FAIL: Could not download source $source_file from $url"
    echo "skipping.."
  fi
  cd ../
}
