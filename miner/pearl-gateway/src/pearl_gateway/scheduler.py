import asyncio
import contextlib
import time

from miner_utils import get_logger

from pearl_gateway.config import PearlConfig
from pearl_gateway.pearl_client import PearlNodeClient
from pearl_gateway.work_cache import WorkCache

logger = get_logger(__name__)


class TemplateScheduler:
    """
    Scheduler for refreshing the block template from the Pearl node.
    Handles both periodic and event-driven refresh strategies.
    """

    def __init__(
        self,
        pearl_client: PearlNodeClient,
        work_cache: WorkCache,
        config: PearlConfig,
    ):
        self.pearl_client = pearl_client
        self.work_cache = work_cache
        self.refresh_interval = config.refresh_interval_seconds
        self.running = False
        self.last_refresh_time = 0
        self.refresh_task: asyncio.Task | None = None

    async def start(self):
        """Start the template refresh scheduler."""
        if self.running:
            return

        self.running = True
        # Immediate initial refresh
        await self.refresh_template()

        # Start the periodic refresh task
        self.refresh_task = asyncio.create_task(self._periodic_refresh())
        logger.info(f"Template scheduler started with refresh interval {self.refresh_interval}s")

    async def stop(self):
        """Stop the template refresh scheduler."""
        self.running = False
        if self.refresh_task:
            self.refresh_task.cancel()
            with contextlib.suppress(asyncio.CancelledError):
                await self.refresh_task
            self.refresh_task = None
        logger.info("Template scheduler stopped")

    async def _periodic_refresh(self):
        """Periodically refresh the template at the configured interval."""
        try:
            while self.running:
                try:
                    # Sleep until next refresh
                    await asyncio.sleep(self.refresh_interval)

                    # Check if we're still running after sleep
                    if not self.running:
                        break

                    # Refresh the template
                    await self.refresh_template()

                except asyncio.CancelledError:
                    break
                except Exception as e:
                    logger.error(f"Error in periodic template refresh: {e}")
                    # Sleep a bit before retrying to avoid tight failure loops
                    await asyncio.sleep(1)
        except asyncio.CancelledError:
            # Task was cancelled, this is expected during shutdown
            pass
        finally:
            # Ensure running is set to False when the task exits
            self.running = False

    async def refresh_template(self) -> bool:
        """
        Refresh the block template from the Pearl node.
        Returns True if a new template was fetched, False otherwise.
        """
        try:
            logger.debug("Refreshing block template")

            # Get latest template from Pearl node
            template = await self.pearl_client.get_block_template()

            # Update the work cache
            updated = await self.work_cache.update_template(template)

            # Update refresh metrics
            self.last_refresh_time = time.time()

            if updated:
                logger.info(f"Template refreshed successfully (height: {template.height})")
            else:
                logger.debug("Template refresh completed (no changes)")

            return updated

        except Exception as e:
            logger.error(f"Failed to refresh template: {e}")
            return False

    def get_time_from_last_refresh(self) -> float | None:
        """Get the time since the last template refresh."""
        if self.last_refresh_time > 0:
            return time.time() - self.last_refresh_time
        else:
            return None
