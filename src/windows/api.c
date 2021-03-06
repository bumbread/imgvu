
struct t_directory_state_s;

internal void platform_directory_set(t_directory_state* dirState, t_string16 path) {
  
  if(!string16_compare(path, dirState->dirPath)) {
    win32_directory_free_files(dirState);
    
    if(dirState->dirSearchPath.ptr) free(dirState->dirSearchPath.ptr);
    if(dirState->dirPath.ptr) free(dirState->dirPath.ptr);
    
    dirState->dirPath = path;
    
    dirState->dirSearchPath = win32_make_path_wildcard_mem(dirState->dirPath);
    win32_directory_scan(dirState);
  }
}

internal void platform_directory_next_file(t_directory_state* dirState) {
  if(dirState->currentFile != 0) {
    win32_set_current_file(dirState, dirState->currentFile->next);
    win32_cache_update(dirState);
  }
}

internal void platform_directory_previous_file(t_directory_state* dirState) {
  if(dirState->currentFile != 0) {
    win32_set_current_file(dirState, dirState->currentFile->prev);
    win32_cache_update(dirState);
  }
}

internal t_image* platform_get_current_image(t_directory_state* dirState) {
  if(dirState->currentFile != 0) {
    t_file* file = dirState->currentFile;
    if(file->image.pixels != 0) {
      return(&file->image);
    }
  }
  return(0);
}

internal void platform_clear_screen(u32 color) {
  //gdi_clear_screen(color);
  gl_clear_screen(color);
}

internal void platform_draw_image(t_location* loc, t_image* image) {
  //gdi_draw_image(loc, image);
  gl_draw_image(loc, image);
}

internal void platform_show(void) {
  //gdi_show();
  gl_show();
}

internal t_file_data platform_load_file(t_string16 fullFilename) {
  t_file_data fileData = {0};
  fileData.filename = fullFilename;
  
  HANDLE fileHandle = CreateFileW((LPCWSTR)fileData.filename.ptr, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
  if(fileHandle != INVALID_HANDLE_VALUE) {
    LARGE_INTEGER fileSize;
    bool result = GetFileSizeEx(fileHandle, &fileSize);
    assert(result);
    if(fileSize.LowPart != 0) {
      byte *bytes = malloc((u32)fileSize.LowPart);
      DWORD bytesRead = 0;
      
      result = ReadFile(fileHandle, bytes, fileSize.LowPart, &bytesRead, 0);
      assert(result);
      CloseHandle(fileHandle);
      assert((DWORD)fileSize.LowPart == bytesRead);
      
      fileData.size = (u32)fileSize.LowPart;
      fileData.ptr = bytes;
    }
  }
  
  return(fileData);
}

internal bool platform_write_file(t_file_data file) {
  HANDLE fileHandle = CreateFileW((LPCWSTR)file.filename.ptr, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
  if(fileHandle != INVALID_HANDLE_VALUE) {
    DWORD bytesWritten;
    bool result = WriteFile(fileHandle, file.ptr, (DWORD)file.size, &bytesWritten, 0);
    if(result == true) {
      if(bytesWritten == (DWORD)file.size) {
        return(true);
      }
    } 
  }
  
  return(false);
}

internal t_string16 platform_get_config_filename(void) {
  DWORD stringSize = 0;
  if(!GetAllUsersProfileDirectoryW(0, &stringSize)) {
    t_string16 directory;
    directory.len = stringSize-1;
    directory.ptr = malloc(stringSize * sizeof(char16));
    if(GetAllUsersProfileDirectoryW((LPWSTR)directory.ptr, &stringSize)) {
      win32_remove_trailing_backslash(&directory);
      
      static_make_string16(filename, L"\\imgvu_config.txt");
      t_string16 fullName = string16_concatenate_mem(directory, filename);
      
      return(fullName);
    }
    else {
      assert(0);
    }
  }
  else {
    assert(0);
  }
  return((t_string16) {0});
}
