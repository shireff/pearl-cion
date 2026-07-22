// Copyright (c) 2025-2026 The Pearl Research Labs
// Use of this source code is governed by an ISC
// license that can be found in the LICENSE file.

package btcutil

const (
	// GrainPerPearlCent is the number of grains in one pearl cent.
	GrainPerPearlCent = 1e6

	// GrainPerPearl is the number of grains in one pearl (1 PRL).
	GrainPerPearl = 1e8

	// MaxGrain is the maximum transaction amount allowed in grains.
	MaxGrain = 21e9 * GrainPerPearl
)
