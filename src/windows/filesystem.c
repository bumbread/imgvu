
#if defined(__TINYC__)
BOOL GetFileSizeEx(HANDLE fileHandle, PLARGE_INTEGER res) {
  res->LowPart = GetFileSize(fileHandle, &res->HighPart);
  return((res->HighPart == 0) && (res->LowPart == INVALID_FILE_SIZE));
}
#endif

internal t_string16 win32_get_current_directory_mem(void) {
  t_string16 result;
  result.len = GetCurrentDirectory(0, 0);
  
  assert(result.len != 0);
  result.ptr = malloc(result.len * sizeof(char16));
  GetCurrentDirectory(result.len, result.ptr);
  
  result.len -= 1;
  return(result);
}

internal t_string16 win32_make_path_wildcard_mem(t_string16 fullPath) {
  t_string16 result;
  result.len = fullPath.len+2;
  result.ptr = malloc((result.len+1) * sizeof(char16));
  for(u32 index = 0; index < fullPath.len; index += 1) result.ptr[index] = fullPath.ptr[index];
  result.ptr[fullPath.len+0] = L'\\';
  result.ptr[fullPath.len+1] = L'*';
  result.ptr[fullPath.len+2] = 0;
  return(result);
}

internal t_string16 win32_get_file_path_mem(t_string16 name) {
  persist t_string16 prepend = {0};
  if(!prepend.len) prepend = char16_copy(L"\\\\?\\");
  
  // NOTE(bumbread): making sure there's no \\?\.
  if(string16_begins_with(name, prepend)) {
    name.len -= 4;
    name.ptr += 4;
  }
  
  // NOTE(bumbread): running the command with empty buffer to get the receive length
  u32 receiveLength = (u32)GetFullPathNameW((LPCWSTR)name.ptr, 0, 0, 0);
  t_string16 result;
  if(receiveLength == 0) {
    result.len = 0;
    result.ptr = 0;
    return(result);
  }
  // NOTE(bumbread): reveiving the actual full path
  char16* fullName = malloc(receiveLength * sizeof(char16));
  GetFullPathNameW((LPCWSTR)name.ptr, receiveLength, fullName, 0);
  result = char16_count(fullName);
  return(result);
}

internal void win32_remove_trailing_backslash(t_string16* path) {
  if(path->ptr[path->len-1] == L'\\') {
    path->len -= 1;
    path->ptr[path->len] = 0;
  }
}

internal t_string16 win32_get_file_path_from_relative_mem(t_string16 directory, t_string16 name) {
  static_make_string16(slash, L"\\");
  t_string16 pathWithSlash = string16_concatenate_mem(directory, slash);
  t_string16 path = string16_concatenate_mem(pathWithSlash, name);
  t_string16 fullPath = win32_get_file_path_mem(path);
  free(path.ptr);
  free(pathWithSlash.ptr);
  return(fullPath);
}

internal t_string16 win32_get_path_to_file_mem(t_string16 fullPath) {
  t_string16 result = string16_copy_mem(fullPath);
  u32 charIndex = fullPath.len - 1;
  loop {
    if(result.ptr[charIndex] == L'\\') {
      result.ptr[charIndex] = 0;
      result.len = charIndex;
      break;
    }
    if(charIndex == 0) break;
    charIndex -= 1;
  }
  
  return(result);
}

internal t_string16 win32_get_path_mem(t_string16 fullPath) {
  t_string16 result = {0};
  
  win32_remove_trailing_backslash(&fullPath);
  WIN32_FIND_DATAW findData = {0};
  HANDLE searchHandle = FindFirstFileW((LPCWSTR)fullPath.ptr, &findData);
  if(searchHandle != INVALID_HANDLE_VALUE) {
    bool isDirectory = ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
    if(isDirectory) result = string16_copy_mem(fullPath);
    else result = win32_get_path_to_file_mem(fullPath);
  }
  return(result);
}

#if 0
internal t_string16 win32_get_file_extension(t_string16 name) {
  u32 charIndex = name.len;
  t_string16 result = {0};
  loop {
    if(name.ptr[charIndex] == L'.') {
      u32 symbolsBeforeExtension = (charIndex+1);
      result.ptr = name.ptr + symbolsBeforeExtension;
      result.len = name.len - symbolsBeforeExtension;
      return(result);
    }
    else if(name.ptr[charIndex] == L'\\') {
      result.ptr = name.ptr + name.len;
      result.len = 0;
    }
    if(charIndex == 0) break;
    charIndex -= 1;
  }
  return(result);
}

internal t_string16 win32_get_dir_level_mem(t_string16 filename, u32 level) {
  t_string16 result;
  result.len = 0;
  u32 currentLevel = 0;
  for(u32 charIndex = 0; charIndex < filename.len; charIndex += 1) {
    if(filename.ptr[charIndex] == '\\') { 
      if(currentLevel == level) {
        break;
      }
      currentLevel += 1;
    }
    result.len += 1;
  }
  result.ptr = malloc((result.len + 1) * sizeof(char16));
  for(u32 charIndex = 0; charIndex < result.len; charIndex += 1) {
    result.ptr[charIndex] = filename.ptr[charIndex];
  }
  result.ptr[result.len] = 0;
  return(result);
}

internal bool directory_contains(t_string16 root, t_string16 dir) {
  return(string_begins_with(dir, root));
}
#endif
