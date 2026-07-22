import { getCurrentNetwork, type Network } from '../config/network-config';

const BlockbookBaseUrlMap: Record<Network, string> = {
    testnet: 'http://blockbook.testnet.pearlresearch.ai',
    mainnet: 'http://blockbook.pearlresearch.ai',
};

function getBaseUrl(): string {
    return BlockbookBaseUrlMap[getCurrentNetwork()];
}

export const BlockbookClient = {
    async estimateFee(numBlocks: number) {
        const response = await fetch(`${getBaseUrl()}/api/v1/estimatefee/${numBlocks}`);
        const data = await response.json();
        return data.result;
    },
};
