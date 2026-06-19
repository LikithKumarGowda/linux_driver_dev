# 09 - DMA (Direct Memory Access)

## Status: Concepts only — not implemented in this environment

DMA requires a real hardware DMA controller wired to specific memory
regions. QEMU's vexpress-a9 emulation has limited DMA controller support,
so a working DMA driver could not be built and tested here. This lesson
covers the concepts so the gap is documented rather than skipped silently.

## What is DMA?

Normally, the CPU moves data byte-by-byte between memory and a device.
With DMA, a dedicated controller moves the data directly between the
device and memory, freeing the CPU to do other work during the transfer.
Without DMA:

Device -> CPU reads each byte -> CPU writes to memory

(CPU is busy for the entire transfer)
With DMA:

Device -> DMA controller -> Memory

(CPU sets up the transfer, then is free until it completes)
## Where DMA is used on real BeagleBone hardware

- SD card / eMMC storage transfers
- Ethernet packet reception
- Audio sample streaming
- Camera sensor frame capture

## Key kernel APIs (for future reference on real hardware)

| API | Purpose |
|---|---|
| `dma_alloc_coherent()` | Allocate a DMA-capable buffer |
| `dma_map_single()` | Map an existing buffer for DMA |
| `dma_unmap_single()` | Unmap after transfer completes |
| `dmaengine_prep_slave_single()` | Prepare a DMA transfer descriptor |
| `dmaengine_submit()` | Queue the transfer |
| `dma_async_issue_pending()` | Start the queued transfer |

## Next steps

Revisit this lesson once physical BeagleBone hardware is available,
where the TI AM335x DMA controller (EDMA) can be used directly.
