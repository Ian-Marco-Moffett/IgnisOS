#include <lib/elf.h>
#include <fs/initrd.h>
#include <lib/log.h>
#include <lib/asm.h>
#include <mm/vmm.h>
#include <mm/mmap.h>
#include <mm/heap.h>

#define ERROR(msg) printk("[KLIB_ELF:%s()]: " msg "\n", __func__);

static uint8_t is_valid(Elf64_Ehdr hdr) {
  if (hdr.e_ident[EI_MAG0] != ELFMAG0) {
    ERROR("EI_MAG0 incorrect (ELF parse error).");
    return 0;
  }

  if (hdr.e_ident[EI_MAG1] != ELFMAG1) {
    ERROR("EI_MAG1 incorrect (ELF parse error).");
    return 0;
  }

  if (hdr.e_ident[EI_MAG2] != ELFMAG2) {
    ERROR("EI_MAG2 incorrect (ELF parse error).");
    return 0;
  }

  if (hdr.e_ident[EI_MAG3] != ELFMAG3) {
    ERROR("EI_MAG3 incorrect (ELF parse error).");
    return 0;
  }

  return 1;
}


static uint8_t is_supported(Elf64_Ehdr hdr) {
  if (hdr.e_ident[EI_CLASS] != ELFCLASS64) {
    ERROR("Unsupported ELF file class.");
    return 0;
  }

  if (hdr.e_ident[EI_DATA] != ELFDATA2LSB) {
    ERROR("Unsupported ELF file byte order.");
    return 0;
  }

  if (hdr.e_machine != EM_X86_64) {
    ERROR("Unsupported ELF file target.");
    return 0;
  }

  if (hdr.e_version != EV_CURRENT) {
    ERROR("Unsupported ELF file version.");
    return 0;
  }

  if (hdr.e_type != ET_EXEC) {
    ERROR("Unsupported ELF file type.");
    return 0;
  } 

  return 1;
}


void* elf_load(const char* initrd_path, program_image_t* pimg) {
  const char* elf_ptr = initrd_open(initrd_path);
  const char* const ORIG_ELF_PTR = elf_ptr;

  if (elf_ptr == NULL) {
    printk(PRINTK_NOTE "[ERROR]: Failed to load \"%s\" from initrd.\n", initrd_path);
    return NULL;
  }

  Elf64_Ehdr header; 

  for (uint64_t i = 0; i < sizeof(header); ++i) {
    ((char*)&header)[i] = elf_ptr[i];
  } 

  if (!(is_valid(header)) || !(is_supported(header))) {
    return NULL;
  }

  const uint64_t PHDRS_SIZE = header.e_phnum*header.e_phentsize;
  Elf64_Phdr* prog_headers = kmalloc(PHDRS_SIZE); 

  elf_ptr = (char*)ORIG_ELF_PTR + header.e_phoff;
  pimg->phdrs = prog_headers;
  pimg->elf_ptr = elf_ptr; 

  for (uint64_t i = 0; i < PHDRS_SIZE; ++i) {
    ((char*)prog_headers)[i] = elf_ptr[i];
  } 

  for (Elf64_Phdr* phdr = prog_headers; (char*)phdr < (char*)prog_headers + header.e_phnum * header.e_phentsize; phdr = (Elf64_Phdr*)((char*)phdr + header.e_phentsize)) {
    if (phdr->p_type == PT_LOAD) {
      int n_pages = (phdr->p_memsz + 0x1000 - 1);
      Elf64_Addr segment = phdr->p_paddr;

      elf_ptr = (char*)ORIG_ELF_PTR + phdr->p_offset;
      mmap((void*)segment, n_pages, PROT_READ | PROT_WRITE | PROT_USER | PROT_EXEC);
            
      for (uint64_t i = 0; i < phdr->p_filesz; ++i) {
        ((char*)segment)[i] = elf_ptr[i];
      }
    }
  }

  void(*entry)(void) = ((__attribute__((sysv_abi))void(*)(void))header.e_entry);
  return entry;
}
